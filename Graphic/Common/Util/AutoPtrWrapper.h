#ifndef __AUTO_PTR_WRAPPER_
#define __AUTO_PTR_WRAPPER_

template <typename T>
class AutoPtrArrayWrapper
{
private:
    T* wrapper;
public:
    AutoPtrArrayWrapper(T* ptr)
    {
        Wrap(ptr);
    }

    ~AutoPtrArrayWrapper()
    {
        if (wrapper!=NULL) 
            delete []wrapper;
    }

    void Wrap(T* ptr)
    {
        wrapper = ptr;
    }

    T* Get()
    {
        return wrapper;
    }
};

class AutoPtrMemoryWrapper
{
private:
    VOID* wrapper;

public:
	AutoPtrMemoryWrapper() : wrapper(NULL)
	{
	}

    AutoPtrMemoryWrapper(VOID* ptr)
    {
        Set(ptr);
    }

    ~AutoPtrMemoryWrapper()
    {
        if (wrapper!=NULL) 
            free(wrapper);
    }

    void Set(VOID* ptr)
    {
        wrapper = ptr;
    }

    VOID* Get()
    {
        return wrapper;
    }

	VOID Reset(VOID* ptr)
	{
		if (wrapper!=NULL) 
			free(wrapper);
		Set(ptr);
	}

};

#endif
