#pragma once
#include<iostream>
#include<string>

template<typename T>
class HEAP {
	unsigned long long size;
	unsigned long long remainingSize;

	char* heap;

	unsigned short headerSize;
	unsigned short footerSize;
	
	void updateRemainingSize(unsigned int addedSize) {
		remainingSize -= 16; // header and footer
		remainingSize -= addedSize;
		return;
	}

	bool hasEnoughCapacity(unsigned int blockSize) const{
		if (remainingSize >= blockSize - 16)
			return true;
		return false;
	}

	unsigned int getSizeOfBlock(unsigned int start) const{
		unsigned int sizeOfBlock = *(unsigned int*)(&heap[start]) >> 1 << 1;
		return sizeOfBlock;
	}

	unsigned int getOffset(unsigned int start) const {
		unsigned int offset = *(unsigned int*)(&heap[start + sizeof(unsigned int)]);
		return offset;
	}

	bool isBlockFree(unsigned int start) {
		unsigned int sizeOfBlock = *(unsigned int*)(&heap[start]);
		return !(sizeOfBlock & 1);
	}

	// No free block if returns -1
	long long nextFreeBlock(unsigned int start) {
		while (start < size) {
			if (isBlockFree(start))
				return start;
			start += getSizeOfBlock(start) + headerSize + footerSize;
		}
		return -1;
	}

	void setHeaderAndFooter(unsigned int start, unsigned int blockSize, bool isAllocated = true) {
		// Size
		*(unsigned int*)(&heap[start]) = blockSize;
		// Flag
		if(isAllocated)
			*(unsigned int*)(&heap[start]) |= 1; // the last bit stores the state (0 = free, 1 = allocated)
		else
			*(unsigned int*)(&heap[start]) &= ~(1); // the last bit stores the state (0 = free, 1 = allocated)


		// Offset
		long long nextFree = nextFreeBlock(start);
		*(unsigned int*)(&heap[start + sizeof(unsigned)]) = nextFree == -1 ? 0: (unsigned)(nextFree);

		// Footer
		// Size
		*(unsigned int*)(&heap[start + blockSize + headerSize]) = blockSize;
		// Flag
		if(isAllocated)
			*(unsigned int*)(&heap[start + blockSize + headerSize]) |= 1;
		else
			*(unsigned int*)(&heap[start + blockSize + headerSize]) &= ~(1);

		updateRemainingSize(isAllocated ? blockSize: 0);
	}
public:
	// Allocate heap with a constant size in bytes
	HEAP(unsigned long long size = 1024) : heap(nullptr) {
		
		this->size = size;
		this->remainingSize = size;

		this->headerSize = 8;
		this->footerSize = 4;


		if (size < 256) {
			std::cerr << "Invalid allocation error: size must be greater than 256" << std::endl;
			exit(1);
		}
		
		heap = new char[size];
		if (heap == nullptr) {
			std::cerr << "Heap allocation was unsuccessful" << std::endl;
			exit(1);
		}

		// Header
		unsigned int sizeOfBlock = size - 16; // size - 16 bytes. 16 bytes for header & footer
		unsigned int offsetToNextFreeBlock = 0; // initially no free block ahead

			// Size
		*(unsigned int*)(&heap[0]) = sizeOfBlock;
			// Flag
		*(unsigned int*)(&heap[0]) &= ~(1 << 0); // 0 = free, 1 = allocated
			// Offset
		*(unsigned int*)(&heap[sizeof(unsigned int)]) = offsetToNextFreeBlock;

		// Footer
			// Size
		*(unsigned int*)(&heap[size - 4]) = sizeOfBlock;
			// Flag
		*(unsigned int*)(&heap[size - 4]) &= ~(1 << 0);

		updateRemainingSize(0);
	}

	//Allocate a single instance 
	void* allocate(T data) {
		unsigned int blockSize = sizeof(data);
		if (!hasEnoughCapacity(blockSize))
			return nullptr;

		int start = 0;
		bool foundUsableBlock = false;

		while (start < size) {
			unsigned int sizeOfBlock = getSizeOfBlock(start);
			unsigned int offset = getOffset(start);
			bool isFree = isBlockFree(start);

			if (!isFree || sizeOfBlock < (blockSize + 16)) {
				start += sizeOfBlock + headerSize + footerSize;
				continue;
			}

			foundUsableBlock = true;
			break;
		}
		if (!foundUsableBlock)
			return nullptr;

		// We now have a usable block at heap[start]
		unsigned int s = getSizeOfBlock(start);
		setHeaderAndFooter(start, blockSize);
		setHeaderAndFooter(start + blockSize + headerSize + footerSize, s - (blockSize + headerSize + footerSize), false);
		return (void*)(heap + start + headerSize);
	}
};

int main() {
	HEAP<int> testHeap(1024);
	return 0;
}