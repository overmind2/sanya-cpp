#ifndef OBJSPACE_INL_HPP
#define OBJSPACE_INL_HPP

namespace sanya {

ObjSpace::ObjSpace()
    : symbol_table_(RawDict::Wrap()) { }

RawSymbol *ObjSpace::InternSymbol(const Handle &symbol) {
    RawPair *rp = symbol_table_.AsDict().LookupSymbol(symbol, kCreateOnAbsent);
    return (RawSymbol *)rp->car();
}

RawSymbol *ObjSpace::InternSymbol(const char *s) {
    Handle symbol = RawSymbol::Wrap(s);
    return InternSymbol(symbol);
}

}  // namespace sanya

// vim: set ts=4 sw=4 sts=4:


#endif /* OBJSPACE_INL_HPP */
