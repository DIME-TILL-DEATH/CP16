int isascii(int c) { return (c & 0x80) == 0; }
int isblank(int c) { return c == ' ' || c == '\t'; }
int isdigit(int c) { return c >= '0' && c <= '9'; }
int iscntrl(int c) { return c < 32; }
int islower(int c) { return c >= 'a' && c <= 'z'; }
int isspace(int c) { return c == ' ' || c == '\f' || c == '\n' || c == '\r' || c == '\t' || c == '\v'; }
int isupper(int c) { return c >= 'A' && c <= 'Z'; }
int isxdigit(int c) { return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'); }

int isalpha(int c) { return isupper(c) || islower(c); }
int isalnum(int c) { return isalpha(c) || isdigit(c); }
int isgraph(int c) { return !iscntrl(c) && !isspace(c); }
int isprint(int c) { return !iscntrl(c); }
int ispunct(int c) { return !iscntrl(c) && !isspace(c) && !isalnum(c); }

int toupper(int c) { return islower(c) ? c & ~0x20 : c; }
int tolower(int c) { return isupper(c) ? c | 0x20 : c; }
