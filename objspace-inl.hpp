
ObjSpace::ObjSpace()
  : symbol_table_(NULL)
{
  symbol_table_ = RawDict::Wrap
}

RawSymbol *ObjSpace::InternSymbol(const Handle &symbol) {
  
}

RawSymbol *ObjSpace::InternSymbol(const char *s) {
  Handle symbol = RawSymbol::Wrap(s);
  return InternSymbol(symbol);
}

