#ifndef TECHARENA2026_BITARRAY_H
#define TECHARENA2026_BITARRAY_H

#include <iostream>

template<typename Align = size_t, size_t AlignSize = sizeof(Align) * 8>
class BitArray
{
private:
    size_t sizeInBits; //Размер битового массива в битах
    size_t alignedSize; //Размер битового массива в единицах выделенной памяти
    Align *data;

    size_t getIndexReverse(size_t n) const{
        return AlignSize - n % AlignSize - 1;
    }
public:
    /**
*   @Method     : BitArray - constructor
*   @Description: This is the BitArray constructor.
*   @Parameters : None
*   @Returned   : None
***************************************************************************/
    BitArray():
            sizeInBits(0),
            alignedSize(0),
            data(nullptr){}

    /**
*   @Method     : ~BitArray - destructor
*   @Description: This is the BitArray destructor.  At this point it's
*                just a place holder.
*   @Parameters : None
*   @Effects    : None
*   @Returned   : None
***************************************************************************/
    ~BitArray() {
        if(this->alignedSize) {
            delete[] this->data;
            this->data = nullptr;
            this->sizeInBits = 0;
            this->alignedSize = 0;
        }
    }

    /**
*   @Method     : BitArray - constructor
*   @Description: This is the BitArray constructor.  It reserves memory
*                for the vector storing the array.
*   @Parameters : num_bits - number of bits in the array
*   @Effects    : Allocates vector for array bits
*   @Returned   : None
***************************************************************************/
    explicit BitArray(size_t num_bits, unsigned long value = 0):
            sizeInBits(num_bits),
            alignedSize((num_bits + AlignSize - 1) / AlignSize),
            data(new Align[alignedSize])
    {
        for(size_t i = 0; i < this->alignedSize; i++){
            this->data[i] = 0;
        }
        for(size_t i = 0, j = static_cast<size_t> (1) << (8 * sizeof(unsigned long) - 1); i < num_bits && i < 8 * sizeof(unsigned long); i++, j >>= 1){
            set(i, value & j);
        }
    }

    /**
*   @Method     : BitArray - copy constructor
*   @Description: This is the BitArray copy constructor.  It copy other BitArray.
*   @Parameters : BitArray & b - BitArray references
*   @Effects    : Allocates vector for array bits
*   @Returned   : None
***************************************************************************/
    BitArray(const BitArray& b):
            sizeInBits(b.sizeInBits),
            alignedSize(b.alignedSize),
            data(new Align[alignedSize]{}) {
        std::copy(b.data, b.data + b.alignedSize, data);
    }


    /**
*   @Method     : swap() - function
*   @Description: exchanging values between source bit array and this array
*   @Parameters : b - Source bit array reference
*   @Effects    : Actual variable renaming
*   @Returned   : Reference to this array after operations
***************************************************************************/
    void swap(BitArray& b){
        std::swap(sizeInBits, b.sizeInBits);
        std::swap(alignedSize, b.alignedSize);
        std::swap(data, b.data);
    }

    /**
*   @Method     : operator=
*   @Description: overload of the = operator.  Copies source contents into
*                this bit array.
*   @Parameters : b - Source bit array reference
*   @Effects    : Source bit array contents are copied into this array
*   @Returned   : Reference to this array after copy
***************************************************************************/
    BitArray& operator=(const BitArray& b){
        if(this == &b){
            return *this;
        }
        this->resize(b.sizeInBits);
        for(size_t i = 0; i < this->alignedSize; i++){
            this->data[i] = b.data[i];
        }
        return *this;
    }

    /**
*   @Method     : resize() - function
*   @Description: Changes the size of the array. In case of expansion, new
*                elements initialized with value.
*   @Parameters : num_bits - new size
*   @Effects    : New size of bitset
*   @Returned   : None
***************************************************************************/
    void resize(size_t num_bits, bool value = false) {
        if(alignedSize == (num_bits + AlignSize - 1) / AlignSize){
            sizeInBits = num_bits;
            return;
        }
        BitArray other(num_bits);
        for(size_t i = 0; i < other.sizeInBits; i++){
            if((i < this->sizeInBits && (*this)[i]) || (i >= this->sizeInBits && value)){
                other.set(i);
            }
        }
        this->swap(other);
    }

    /**
*   @Method     : clear() - function
*   @Description: This method delete data.
*   @Parameters : None
*   @Effects    : delete data and set data = nullptr, set size of bit array = 0.
*   @Returned   : None
***************************************************************************/
    void clear() {
        if(alignedSize){
            alignedSize = 0;
            sizeInBits = 0;
            delete[] data;
            data = nullptr;
        }
    }

    /**
*   @Method     : push_back() - function
*   @Description: adds a new element to the end of the bitset
*   @Parameters : bit - new element value
*   @Effects    : None
*   @Returned   : None
***************************************************************************/
    void push_back(bool bit) {
        resize(sizeInBits + 1);
        if(bit){
            set(sizeInBits - 1);
        }
        else{
            reset(sizeInBits - 1);
        }
    }

    /**
*   @Method     : operator&=
*   @Description: overload of the &= operator.  Performs a bitwise and
*                between the source array and this bit array.  This bit
*                array will contain the result.
*   @Parameters : b - Source bit array reference
*   @Effects    : Results of bitwise and are stored in this array
*   @Returned   : Reference to this array after and
***************************************************************************/
    BitArray& operator&=(const BitArray& b) {
        if(this->sizeInBits != b.sizeInBits){
            std::cerr << "operator&=: different size of arrays\n";
            return *this;
        }
        for(size_t i = 0; i < this->alignedSize; i++){
            this->data[i] &= b.data[i];
        }
        return *this;
    }

    /**
*   @Method     : operator|=
*   @Description: overload of the |= operator.  Performs a bitwise xor
*                between the source array and this bit array.  This bit
*                array will contain the result.
*   @Parameters : b - Source bit array reference
*   @Effects    : Results of bitwise xor are stored in this array
*   @Returned   : Reference to this array after or
***************************************************************************/
    BitArray& operator|=(const BitArray& b){
        if(this->sizeInBits != b.sizeInBits){
            std::cerr << "operator|=: different size of arrays\n";
            return *this;
        }
        for(size_t i = 0; i < this->alignedSize; i++){
            this->data[i] |= b.data[i];
        }
        return *this;
    }

    /**
*   @Method     : operator^=
*   @Description: overload of the ^= operator.  Performs a bitwise xor
*                between the source array and this bit array.  This bit
*                array will contain the result.
*   @Parameters : b - Source bit array reference
*   @Effects    : Results of bitwise xor are stored in this array
*   @Returned   : Reference to this array after xor
***************************************************************************/
    BitArray& operator^=(const BitArray& b) {
        if(this->sizeInBits != b.sizeInBits){
            std::cerr << "operator^=: different size of arrays\n";
            return *this;
        }
        for(size_t i = 0; i < this->alignedSize; i++){
            this->data[i] ^= b.data[i];
        }
        return *this;
    }

    /**
*   @Method     : operator<<=
*   @Description: overload of the <<= operator.  Performs a left shift on
*                this bit array.  This bit array will contain the result.
*   @Parameters : n - number of bit positions to shift
*   @Effects    : Results of the shifts are stored in this array
*   @Returned   : Reference to this array after shift
***************************************************************************/
    BitArray& operator<<=(size_t n){
        if(this->alignedSize == 0){
            return *this;
        }
        for(size_t i = 0; i < sizeInBits; i++){
            reset(i);
            if(i + n < sizeInBits && (*this)[i + n]) {
                set(i);
            }
        }
        return *this;
    }

    /**
*   @Method     : operator>>=
*   @Description: overload of the >>= operator.  Performs a right shift on
*                this bit array.  This bit array will contain the result.
*   @Parameters : n - number of bit positions to shift
*   @Effects    : Results of the shifts are stored in this array
*   @Returned   : Reference to this array after shift
***************************************************************************/
    BitArray& operator>>=(size_t n){
        if(this->alignedSize == 0){
            return *this;
        }
        for(size_t i = sizeInBits; i > 0; i--){
            reset(i - 1);
            if(i - 1 >= n && (*this)[i - 1 - n]){
                set(i - 1);
            }
        }
        return *this;
    }

    /**
*   @Method     : operator<<
*   @Description: overload of the << operator.  Performs a bitwise left
*                shift of this bit array.
*   @Parameters : n - the number of bits to shift left
*   @Effects    : None
*   @Returned   : result of bitwise left shift
***************************************************************************/
    BitArray operator<<(size_t n) const {
        BitArray other(*this);
        other <<= n;
        return other;
    }

    /**
*   @Method     : operator>>
*   @Description: overload of the >> operator.  Performs a bitwise right
*                shift of this bit array.
*   @Parameters : n - the number of bits to shift right
*   @Effects    : None
*   @Returned   : result of bitwise right shift
***************************************************************************/
    BitArray operator>>(size_t n) const {
        BitArray other(*this);
        other >>= n;
        return other;
    }

    /**
*   @Method     : set
*   @Description: This method sets one bit in the bit array to 1.
*   @Parameters : n - the bit number to be set true.
*   @Effects    : Bit with number n is set to 1.
*   @Returned   : BitArray &
***************************************************************************/
    BitArray& set(size_t n, bool value = true){
        if(!value){
            reset(n);
            return *this;
        }
        if(n > sizeInBits){
            std::cerr << "stack overflow (set(n))\n";
            return *this;
        }
        data[n / AlignSize] |= (static_cast<Align> (1) << getIndexReverse(n));
        return *this;
    }

    /**
*   @Method     : set
*   @Description: This method sets every bit in the bit array to 1.
*   @Parameters : None
*   @Effects    : Each of the bits used in the bit array are set to 1.
*   @Returned   : BitArray &
***************************************************************************/
    BitArray& set(){
        Align pat = -1;
        for(size_t i = 0; i < alignedSize; i++){
            data[i] = pat;
        }
        return *this;
    }

    /**
*   @Method     : reset
*   @Description: This method sets one bit in the bit array to 0.
*   @Parameters : n - number
*   @Effects    : Bit with number n is set to 0.
*   @Returned   : BitArray &
***************************************************************************/
    BitArray& reset(size_t n){
        if(n > sizeInBits){
            std::cerr << "stack overflow (reset(n))\n";
            return *this;
        }
        Align help = getIndexReverse(n) + 1;
        Align pat = ~(static_cast<Align> (1) << (help - 1));
        data[n / AlignSize] &= pat;
        return *this;
    }

    /**
*   @Method     : reset
*   @Description: This method sets every bit in the bit array to 1.
*   @Parameters : None
*   @Effects    : Each of the bits used in the bit array are set to 0.
*   @Returned   : BitArray &
***************************************************************************/
    BitArray& reset(){
        for(size_t i = 0; i < this->alignedSize; i++){
            data[i] = 0;
        }
        return *this;
    }

    /**
*   @Method     : any() - function
*   @Description: checks if all bits in a bitset are false
*   @Parameters : None
*   @Effects    : None
*   @Returned   : false is all bits are false
***************************************************************************/
    [[nodiscard]] bool any() const{
        for(size_t i = 0; i < alignedSize - 1; i++){
            if(data[i] > 0){
                return true;
            }
        }
        Align help = getIndexReverse(sizeInBits) + 1;
        Align pat = 0;
        for(size_t i = help, j = static_cast<size_t> (1) << help; i < AlignSize; i++, j <<= 1){
            pat |= j;
        }
        return (data[alignedSize - 1] & pat) > 0;
    }

    /**
*   @Method     : none() - function
*   @Description: checks if all bits in a bitset are true
*   @Parameters : None
*   @Effects    : None
*   @Returned   : true is all bits are false
***************************************************************************/
    [[nodiscard]] bool none() const{
        return !this->any();
    }

    /**
*   @Method     : operator~
*   @Description: overload of the ~ operator.  Negates all non-spare bits in
*                bit array
*   @Parameters : None
*   @Effects    : None
*   @Returned   : value of this after bitwise not
***************************************************************************/
    BitArray operator~() const{
        BitArray other(*this);
        for(size_t i = 0; i < this->alignedSize; i++){
            Align pat = -1;
            other.data[i] ^= pat;
        }
        return other;
    }

    /**
*   @Method     : count() - function
*   @Description: The function counts the number of true bits
*   @Parameters : None
*   @Effects    : None
*   @Returned   : count true bits
***************************************************************************/
    [[nodiscard]] size_t count() const{
        size_t counter = 0;
        for(size_t i = 0; i < sizeInBits; i++) {
            if((*this)[i]){
                counter++;
            }
        }
        return counter;
    }

    /**
*   @Method     : operator[]
*   @Description: Overload of the [] operator.  This method returns the
*                value of a bit in the bit array.
*   @Parameters : i - index of array bit
*   @Effects    : None
*   @Returned   : The value of the specified bit.
***************************************************************************/
    bool operator[](size_t i) const{
        Align pat = static_cast<size_t> (1) << getIndexReverse(i);
        return (data[i / AlignSize] & pat);
    }

    /**
*   @Method     : size() - function
*   @Description: The function which return size of bit array
*   @Parameters : None
*   @Effects    : None
*   @Returned   : size_t - size of bit array
***************************************************************************/
    [[nodiscard]] size_t size() const{
        return sizeInBits;
    }

    /**
*   @Method     : empty() - function
*   @Description: returned bool value: if size of bit array isn't null - false, else - true
*   @Parameters : None
*   @Effects    : None
*   @Returned   : bool - true if size of bit array is null
***************************************************************************/
    [[nodiscard]] bool empty() const {
        return sizeInBits == 0;
    }

    /**
*   @Method     : to string() - function
*   @Description: Returns a string representation of an array
*   @Parameters : None
*   @Effects    : None
*   @Returned   : A string representation of an array
***************************************************************************/
    [[nodiscard]] std::string to_string() const {
        std::string bytes;
        char symbol = '\0';
        for(size_t i = 0; i < sizeInBits; i += 8){
            for(size_t j = 0; j < 8 && i + j < sizeInBits; j++){
                if((*this)[i + j]){
                    symbol |= static_cast<char> (1) << (7 - j % 8);
                }
            }
            bytes.push_back(symbol);
            symbol = '\0';
        }
        return bytes;
    }

    
};


/**
*   @Method     : operator==
*   @Description: overload of the == operator
*   @Parameters : a, b - bit array references to compare
*   @Effects    : None
*   @Returned   : True if this == other.  Otherwise false.
***************************************************************************/
template<typename Align = size_t>
bool operator==(const BitArray<Align> & a, const BitArray<Align> & b) {
    if(a.size() != b.size()){
        return false;
    }
    for(size_t i = 0; i < a.size(); i++){
        if((a[i]) != (b[i])){
            return false;
        }
    }
    return true;
}

/**
*   @Method     : operator!=
*   @Description: overload of the != operator
*   @Parameters : a, b - bit array references to compare
*   @Effects    : None
*   @Returned   : True if this != other.  Otherwise false.
***************************************************************************/
template<typename Align = size_t>
bool operator!=(const BitArray<Align> & a, const BitArray<Align> & b) {
    return !(a == b);
}

/**
*   @Method     : operator&
*   @Description: overload of the & operator.  Performs a bitwise and
*                between the source arrays.
*   @Parameters : b1, b2 - bit array references on righthand side of &
*   @Effects    : None
*   @Returned   : BitArray - result operation &
***************************************************************************/
template<typename Align = size_t>
BitArray<Align> operator&(const BitArray<Align>& b1, const BitArray<Align>& b2) {
    if(b1.size() != b2.size()){
        std::cerr << "operator&: different lengths\n";
        return b1;
    }
    BitArray<Align> other(b1);
    other &= b2;
    return other;
}

/**
*   @Method     : operator|
*   @Description: overload of the | operator.  Performs a bitwise and
*                between the source arrays.
*   @Parameters : b1, b2 - bit array references on righthand side of |
*   @Effects    : None
*   @Returned   : BitArray - result operation |
***************************************************************************/
template<typename Align = size_t>
BitArray<Align> operator|(const BitArray<Align> & b1, const BitArray<Align> & b2) {
    if(b1.size() != b2.size()){
        std::cerr << "operator|: different lengths\n";
        return b1;
    }
    BitArray<Align> other(b1);
    other |= b2;
    return other;
}

/**
*   @Method     : operator^
*   @Description: overload of the ^ operator.  Performs a bitwise and
*                between the source arrays.
*   @Parameters : b1, b2 - bit array references on righthand side of ^
*   @Effects    : None
*   @Returned   : BitArray - result operation ^
***************************************************************************/
template<typename Align = size_t>
BitArray<Align> operator^(const BitArray<Align> & b1, const BitArray<Align> & b2) {
    if(b1.size() != b2.size()){
        std::cerr << "operator^: different lengths\n";
        return b1;
    }
    BitArray<Align> other(b1);
    other ^= b2;
    return other;
}


#endif // TECHARENA2026_BITARRAY_H