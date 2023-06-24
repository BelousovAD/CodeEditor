#include "languageSettings.h"

enum {
    KEYWORDLIST_0 = 0,
    KEYWORDLIST_1,
    KEYWORDLIST_2,
};

wxString O2MWordList0 =
"ARRAY BEGIN BY CASE CONST DIV DO ELSE ELSIF END EXIT FOR "
"IF IMPORT IN IS LOOP MOD MODULE NIL OF OR POINTER PROCEDURE "
"RECORD REPEAT RETURN THEN TO TYPE UNTIL VAR WHILE WITH "
"FALSE TRUE "
"ABS ASH CAP CHR ENTIER LEN LONG MAX MIN ODD ORD SHORT SIZE "
"ASSERT COPY DEC EXCL HALT INC INCL NEW "
"BOOLEAN CHAR INTEGER LONGINT LONGREAL REAL SET SHORTINT "
"LOCAL";

const std::vector<LanguageInfo> g_LanguageSettings = {
    // Default
    {"Default",
     "",
     wxSTC_LEX_CONTAINER,
     {},
     {},
    },
    // O2M
    {"O2M",
     "o",
     wxSTC_LEX_O2M,
     {{wxSTC_O2M_DEFAULT, "face:Consolas,fore:#F1F1F1,back:#1E1E1E,size:13"},
      {wxSTC_O2M_NUMBER, "fore:SIENNA"},
      {wxSTC_O2M_SINGLESTRING, "fore:KHAKI"},
      {wxSTC_O2M_DOUBLESTRING, "fore:KHAKI"},
      {wxSTC_O2M_CHARACTER, "fore:KHAKI"},
      {wxSTC_O2M_KEYWORD, "fore:BLUE"},
      {wxSTC_O2M_BLOCKCOMMENT, "fore:FOREST GREEN"},
      {wxSTC_O2M_LINECOMMENT, "fore:RED"},
     },
     {{KEYWORDLIST_0, O2MWordList0},
     },
    },
};