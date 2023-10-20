
#include <stdint.h>
#include <memory.h>

class MemoryBlockBase
{
protected:
	void* start = nullptr;
	uint32_t bytes;
	bool locked = false;
	static inline size_t handles = 0;
};


template <class T>
class MemoryBlock : MemoryBlockBase
{
	friend class MemoryManager;
	
public:
	T& operator[] (size_t index)
	{
		if (index > count-1)
			return nullptr;
		
		return (T&)start + sizeof(T) * index;
	}
	
private:
	uint32_t count = 0;
	uint32_t bytes = 0;
};

class MemoryManager
{
public:
	MemoryManager(void* start, uint32_t bytes)
	{
		this->start = start;
		this->bytes = bytes;
		this->end = (uint8_t*) start + bytes;
		_free = bytes;
		allocate_blocks(10);
	}
	
	template <class T>
	T* allocate(uint32_t count)
	{
		uint32_t bytes = count * sizeof(T);
		T* ptr = (T*) get_block(bytes);
		if (ptr == nullptr)
			return nullptr;
		
		_free
		return ptr;
	}
	
	void free(void* handle)
	{
		for (size_t i=0; i<block_count; i++)
		{
			MemoryBlockBase* block = blocks[i];
			if (block->start == handle)
			{
				_free += block->bytes;
				if (i < block_count - 1)
					blocks[i] = blocks[i+1];
			}
		}
	}
	
	void* get_block(uint32_t bytes)
	{
		return start;
	}
	
	void defragment(void)
	{
	}
	
	size_t get_free(void)
	{
	}
	
private:
	void allocate_blocks(uint32_t count)
	{
		blocks = allocate<MemoryBlockBase>(count);
	}

	void* start;
	void* end;
	uint32_t bytes;
	uint32_t _free;
	size_t block_count = 0;
	MemoryBlockBase* blocks;
};


int main(void)
{
	static uint8_t heap[0x1000];
	
	MemoryManager manager(heap, 0x1000);
	
	uint8_t* buffer = manager.allocate<uint8_t>(100);
	
	return 0;
}