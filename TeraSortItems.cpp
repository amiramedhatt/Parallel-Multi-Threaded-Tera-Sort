#include "TeraSort.h"
#include "Thread.h"

// Constructor
TeraSortItem::TeraSortItem()
{
    teraitem = new teraitem_r();
}

// Copy Constructor
TeraSortItem::TeraSortItem(TeraSortItem *teraSortItem)
{
    teraitem = new teraitem_r();
    memcpy(teraitem, teraSortItem->getTeraItem(), sizeof(teraitem_r));
}

TeraSortItem::TeraSortItem(teraitem_r *item)
{
    teraitem = new teraitem_r();
    memcpy(teraitem, item, sizeof(teraitem_r));
}

TeraSortItem::TeraSortItem(TeraSortItem &teraSortItem)
{
    teraitem = new teraitem_r();
    memcpy(teraitem, teraSortItem.getTeraItem(), sizeof(teraitem_r));
}

// Extract the key as a 128 bit integer
uint128_t TeraSortItem::key()
{
    uint128_t result = 0;
    for (int i = 0; i < 10; i++)
        result = (result << 8) | teraitem->key[i];
    return result;
}

void TeraSortItem::swap(Sortable *p_sortable)
{
    TeraSortItem *other = (TeraSortItem *)p_sortable;
    TeraSortItem temp = *other;
    this->teraitem = other->teraitem;
    other->teraitem = temp.teraitem;
}

uint16_t TeraSortItem::hash(uint16_t p_reducers)
{
    return (this->key() % p_reducers);
}

teraitem_r *TeraSortItem::getTeraItem()
{
    return teraitem;
}

bool TeraSortItem::operator==(Sortable *p_sortable)
{
    return this->key() == p_sortable->key();
}

bool TeraSortItem::operator!=(Sortable *p_sortable)
{
    return this->key() != p_sortable->key();
}

bool TeraSortItem::operator>(Sortable *p_sortable)
{
    return this->key() > p_sortable->key();
}

bool TeraSortItem::operator<(Sortable *p_sortable)
{
    return this->key() < p_sortable->key();
}

bool TeraSortItem::operator>=(Sortable *p_sortable)
{
    return this->key() >= p_sortable->key();
}

bool TeraSortItem::operator<=(Sortable *p_sortable)
{
    return this->key() <= p_sortable->key();
}

TeraSortItem::~TeraSortItem()
{
    delete teraitem;
}

ShuffleBuffer::ShuffleBuffer(int size)
{
    this->size = size;
    this->current_size = 0;  // FIX: Initialize to 0
    teradata = new teraitem_r[size];
}

ShuffleBuffer::~ShuffleBuffer()
{
    delete[] teradata;
}

ShuffleBuffers::ShuffleBuffers(int count, int size)
{
    this->count = count;
    this->size = size;
    shuffleBuffers = new ShuffleBuffer *[count];
    for (int i = 0; i < count; i++)
    {
        shuffleBuffers[i] = new ShuffleBuffer(size);
    }
}

void ShuffleBuffers::append(int buffer_index, teraitem_r *teradata)
{
    ShuffleBuffer *buf = shuffleBuffers[buffer_index];
    std::lock_guard<std::mutex> lock(buf->getMutex());

    if (buf->getCurrentSize() >= buf->getSize()) {
        std::cerr << "ERROR: Shuffle buffer " << buffer_index << " overflow\n";
        exit(1);   // or throw
    }

    buf->teradata[buf->getCurrentSize()] = *teradata;
    buf->incrementCurrentSize();
}

void ShuffleBuffers::save(char *filename)
{
    ofstream outfile;
    // FIX: Use truncate mode instead of append
    outfile.open(filename, ios::out | ios::binary | ios::trunc);
    if (!outfile)
    {
        cerr << "Error: could not open output file " << filename << endl;
        return;
    }
    
    for (int i = 0; i < count; i++)
    {
        lock_guard<std::mutex> lock(shuffleBuffers[i]->getMutex());
        outfile.write((char *)shuffleBuffers[i]->teradata, sizeof(teraitem_r) * shuffleBuffers[i]->getCurrentSize());
    }
    outfile.close();
}

ShuffleBuffer *ShuffleBuffers::operator[](int index)
{
    if (index < count && index >= 0)
    {
        return shuffleBuffers[index];
    }
    else
        return nullptr;
}

ShuffleBuffers::~ShuffleBuffers()
{
    for (int i = 0; i < count; i++)
    {
        delete shuffleBuffers[i];
    }
    delete[] shuffleBuffers;
}
