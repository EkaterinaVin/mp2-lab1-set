// ННГУ, ВМК, Курс "Методы программирования-2", С++, ООП
//
// tbitfield.cpp - Copyright (c) Гергель В.П. 07.05.2001
//   Переработано для Microsoft Visual Studio 2008 Сысоевым А.В. (19.04.2015)
//
// Битовое поле

#include "tbitfield.h"

// Fake variables used as placeholders in tests
static const int FAKE_INT = -1;
static TBitField FAKE_BITFIELD(1);

//мем это ячейка по условно 8 бит, у нас есть массив мемов по сколько-то ячеек в нем 

TBitField::TBitField(int len)
{
    if (len < 0) {
        throw (len);
    }
    BitLen = len;
    MemLen = (len - 1) / (sizeof(TELEM) * 8) + 1;
    pMem = new TELEM[MemLen];
    for (int i = 0; i < MemLen; i++)
    {
        pMem[i] = 0;
    }
}

TBitField::TBitField(const TBitField& bf) // конструктор копирования
{
    MemLen = bf.MemLen;
    BitLen = bf.BitLen;
    pMem = new TELEM[MemLen];
    for (int i = 0; i < MemLen; i++)
    {
        pMem[i] = bf.pMem[i];
    }
}

TBitField::~TBitField()
{
    delete[] pMem;
    BitLen = 0;
    MemLen = 0;
}

int TBitField::GetMemIndex(const int n) const // индекс Мем для бита n
{
    if (n<0 || n>BitLen) 
    { 
        throw n; 
    }
    return (n / (sizeof(TELEM) * 8));
}

TELEM TBitField::GetMemMask(const int n) const // битовая маска для бита n //мемовая маска это представление числа как 0 и 1, но то 
                                                                               //как она составляется я так и не поняла 
{
    if ((n < 0) || (n >= BitLen)) { throw n; }
    else
    {
        return 1 << (n - 1) % (8 * sizeof(TELEM));
    }

}

// доступ к битам битового поля

int TBitField::GetLength(void) const // получить длину (к-во битов)
{

    return BitLen;
}

void TBitField::SetBit(const int n) // установить бит //нерабочая штука нужно, чтобы заработала
{
    if (n < 0 || n >= BitLen) {
        throw n;
    }
    int index = GetMemIndex(n);
    TELEM Mask = GetMemMask(n);
    pMem[index] = pMem[index] | Mask;
}

void TBitField::ClrBit(const int n) // очистить бит //проблема в амперсантах и палках перед равно
{
    if (n < 0 || n >= BitLen) {
        throw n;
    }
    pMem[GetMemIndex(n)] = pMem[GetMemIndex(n)] & ~GetMemMask(n); 
}

int TBitField::GetBit(const int n) const // получить значение бита//дописать
{
    if (n < 0 || n >= BitLen) {
        throw n;
    }
    return (pMem[GetMemIndex(n)] & GetMemMask(n));
}

// битовые операции

TBitField& TBitField::operator=(const TBitField& bf) // присваивание
{

    if (&bf == this)
    {
        return *this;
    }

    if (MemLen != bf.MemLen)
    {
        delete[] pMem;
        pMem = new TELEM[MemLen];
        MemLen = bf.MemLen;
    }
    BitLen = bf.BitLen;
    for (int i = 0; i < MemLen; i++) {
        pMem[i] = bf.pMem[i];
    }
    return *this;
}

int TBitField::operator==(const TBitField& bf) const // сравнение
{
    int res = 1;
    if (BitLen != bf.BitLen)
        res = 0;
    else
        for (int i = 0; i < MemLen; i++)
            if (pMem[i] != bf.pMem[i])
            {
                res = 0;
                break;
            }
    return res;
}

int TBitField::operator!=(const TBitField& bf) const // сравнение //можно было проще
{
    return !(*this == bf);


    /*int res = 0;
    if (BitLen != bf.BitLen)
        res = 1;
    else
        for (int i = 0; i < MemLen; i++)
        {
            if (pMem[i] != bf.pMem[i])
            {
                res = 1;
                break;
            }
        }
    return res;*/
}

TBitField TBitField::operator|(const TBitField& bf) // операция "или"
{
    int i, len = BitLen;
    if (bf.BitLen > len) 
        len = bf.BitLen;
    TBitField temp(len);
    for (i = 0; i < MemLen; i++)
        temp.pMem[i] = pMem[i];
    for (i = 0; i < bf.MemLen; i++) 
        temp.pMem[i] |= bf.pMem[i];
    return temp;
}

TBitField TBitField::operator&(const TBitField& bf) // операция "и"//минимум
{
    int l;
    if (bf.BitLen > BitLen) {
        l = bf.BitLen;
    }
    else {
        l = BitLen;
    }
    TBitField Field(l);
    for (int i = 0; i < bf.MemLen; i++)
    {
        Field.pMem[i] = bf.pMem[i];
    }
    for (int i = 0; i < std::min(bf.MemLen, MemLen); i++) {
        Field.pMem[i] = Field.pMem[i] & pMem[i];
    }
    return Field;
}

TBitField TBitField::operator~(void) // отрицание 
{

    TBitField Field(*this);
    for (int i = 0; i < MemLen - 1; i++) {
        Field.pMem[i] = ~Field.pMem[i];
    }
    if (MemLen > 0) {
        for (int i = (MemLen - 1) * sizeof(TELEM) * 8; i < Field.BitLen; i++) {
            if (Field.GetBit(i)) {
                Field.ClrBit(i);
            }
            else
                Field.SetBit(i);
        }
    }
    return Field;
}


istream& operator>>(istream& istr, TBitField& bf) // ввод
{
    int i = 0;
    char ch;
    do { istr >> ch; } while (ch != ' ');
    while (true)
    {
        istr >> ch;
        if (ch == '0')
            bf.ClrBit(i++);
        else if (ch == '1')
            bf.SetBit(i++);
        else break;
    }
    return istr;
}

ostream& operator<<(ostream& ostr, const TBitField& bf) // вывод
{
    for (int i = 0; i < bf.BitLen; i++) {
        if (bf.GetBit(i))
            ostr << 1;
        else
            ostr << 0;
    }
    return ostr;
}