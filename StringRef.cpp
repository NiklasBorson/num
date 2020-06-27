#include "stdafx.h"
#include "StringRef.h"

namespace num {

    void StringRef::AddRef(Data* data)
    {
        ++(data->refCount_);
    }

    void StringRef::Release(Data* data)
    {
        if (--(data->refCount_) == 0)
        {
            free(data);
        }
    }

    StringRef::Data* StringRef::NewData(_In_reads_(length) char const* source, size_t length)
    {
        // Maximum length avoid integer overflow when casting to uint32_t or when
        // computing bufferSize below.
        constexpr size_t maxLength = std::min<size_t>(
            UINT32_MAX,
            (SIZE_MAX / sizeof(*source)) - 1 - offsetof(Data, text_)
            );

        if (length > maxLength)
        {
            throw std::bad_alloc();
        }

        size_t const bufferSize = offsetof(Data, text_) + (length + 1) * sizeof(*source);
        auto data = static_cast<Data*>(malloc(bufferSize));

        data->refCount_ = 0;
        data->length_ = static_cast<uint32_t>(length);
        memcpy(data->text_, source, length * sizeof(*source));
        data->text_[length] = 0;

        return data;
    }

    StringRef::~StringRef()
    {
        Release(data_);
    }

    StringRef::StringRef(_In_z_ char const* text)
    {
        data_ = NewData(text, strlen(text));
        AddRef(data_);
    }

    StringRef::StringRef(gsl::span<char const> text)
    {
        data_ = NewData(text.data(), text.length());
        AddRef(data_);
    }

    StringRef::StringRef(StringRef const& rhs)
    {
        data_ = rhs.data_;
        AddRef(data_);
    }

    void StringRef::operator=(StringRef const& rhs)
    {
        AddRef(rhs.data_);
        Release(data_);
        data_ = rhs.data_;
    }

} // end namespace num
