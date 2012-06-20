//===-- ALFBuilder.h - Creating ALF (Artist2 Language for Flow Analysis) modules --------------===//
//
//                     Benedikt Huber, <benedikt@vmars.tuwien.ac.at>
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef __ALF_CONTEXT_H__
#define __ALF_CONTEXT_H__

#include "SExpr.h"

namespace alf {

/// Configuration for ALF generation
class ALFConfiguration {
    static const unsigned DEFAULT_POINTER_BIT_WIDTH = 32;

    /// Bits FRef, LRef and Offset take
    unsigned BitsFRef, BitsLRef, BitsOffset;

    /// Whether to use macros (always false at the moment)
    bool UseMacros;

    /// Whether this is a little-endian module
    bool IsLittleEndian;

    /// Least Addressable unit (fixed to 8 at the moment)
    unsigned LeastAddrUnit;

public:
    ALFConfiguration(unsigned leastAddrUnit) {
        setBitWidths(DEFAULT_POINTER_BIT_WIDTH,DEFAULT_POINTER_BIT_WIDTH,DEFAULT_POINTER_BIT_WIDTH);
        LeastAddrUnit = leastAddrUnit;
        UseMacros = false;
    }
    void setBitWidths(unsigned bitsLRef, unsigned bitsFRef, unsigned bitsOffset) {
        BitsLRef = bitsLRef;
        BitsFRef = bitsFRef;
        BitsOffset = bitsOffset;
    }
    bool isLittleEndian() {
        return IsLittleEndian;
    }
    void setLittleEndian(bool isLittleEndian) {
        IsLittleEndian = isLittleEndian;
    }
    /// Bits a frameref occupies
    unsigned getBitsFRef()   { return BitsFRef; }

    /// Bits a lablref occupies
    unsigned getBitsLRef()   { return BitsLRef; }

    /// Bits occupied by an offset
    unsigned getBitsOffset() { return BitsOffset; }

    /// Get Least Addressable Unit
    unsigned getLAU() { return LeastAddrUnit; }
};

/// Context for creating ALF expressions
class ALFContext : public SExprContext {
    ALFConfiguration *Config;
public:
    ALFContext(ALFConfiguration* config) : Config(config ){
    }

    /// basic builders
    SExpr* const_repeat(SExpr *Value, uint64_t N) {
        return list("const_repeat")->append(Value)->append(N);
    }
    SExpr* dec_unsigned(unsigned BitWidth, uint64_t Value) {
        return list("dec_unsigned")->append(BitWidth)->append(Value);
    }

    SExpr* identifier(const Twine& Name);

    // FIXME: should be StringRef
    SExpr* address(const Twine& Name, uint64_t OffsetInBits = 0) {
        return list("addr")->append(Config->getBitsFRef())
                            ->append(fref(Name))
                            ->append(offset(OffsetInBits));
    }
    SExpr* fref(const Twine& Name) {
        return list("fref")->append(Config->getBitsFRef())
                            ->append(identifier(Name));
    }
    SExpr* lref(const Twine& Name) {
        return list("lref")->append(Config->getBitsLRef())
                            ->append(identifier(Name));
    }
    // Offset of Address or Label
    // Parameter is in Bits, and converted to LAU
    SExpr* offset(unsigned OffsetInBits) {
        assert(OffsetInBits % Config->getLAU() == 0 && "Addressing Error (unaligned offset)");
        return list("dec_unsigned")
                ->append(Config->getBitsOffset())
                ->append(OffsetInBits / Config->getLAU());
    }
    SExpr* jump(const Twine &Id, uint64_t Offset = 0, uint64_t Leaving = 0) {
        return list("jump")
                ->append(labelRef(Id,Offset))
                ->append("leaving")
                ->append(Leaving);
    }
    // expression and statement construction
    SExpr* load(unsigned BitWidth, SExpr* ref) {
        return list("load")->append(BitWidth)->append(ref);
    }
    SExpr* load(unsigned BitWidth, const Twine& Fref, unsigned Offset = 0) {
        return list("load")->append(BitWidth)->append(address(Fref,Offset));
    }
    SExpr* null() {
        return list("null");
    }
    SExprList* ret() {
        return list("return");
    }
    SExpr* ret(SExpr *ReturnValue) {
        return list("return")->append(ReturnValue);
    }
    SExpr* store(SExpr *Adress, SExpr *Expr) {
        return list("store")->append(Adress)->append("with")->append(Expr);
    }
    SExpr* labelRef(const Twine& Id, unsigned Offset = 0) {
        return list("label")->append(Config->getBitsLRef())
                             ->append(lref(Id))
                             ->append(offset(Offset));
    }
};

} // end namespace alf

#endif
