// Scintilla source code edit control
/**
 * @file LexO2M.cxx
 * @date April 30, 2023
 * @brief Lexer for O2M
 * @author ADBelousov
 *
 * The License.txt file describes the conditions under which this software may
 * be distributed.
 *
 */
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
#include <ctype.h>

#include "ILexer.h"
#include "Scintilla.h"
#include "SciLexer.h"

#include "WordList.h"
#include "LexAccessor.h"
#include "Accessor.h"
#include "StyleContext.h"
#include "CharacterSet.h"
#include "LexerModule.h"

#include <cstdlib>
#include <cassert>
#include <cctype>
#include <cstdio>
#include <string>
#include <vector>
#include <map>
#include <regex>

#include "ILexer.h"
#include "Scintilla.h"
#include "SciLexer.h"

#include "WordList.h"
#include "LexAccessor.h"
#include "StyleContext.h"
#include "CharacterSet.h"
#include "LexerModule.h"

#ifdef SCI_NAMESPACE
using namespace Scintilla;
#endif

static const char *const O2MWordListDesc[] = {
	"O2M Keywords",
	0
};

class LexerO2M : public ILexer {
	WordList keywords0;

    CharacterSet setNumber;
    CharacterSet setCharacter;

	int GetLengthNextWordIfInList(WordList &keywordList, StyleContext &context, LexAccessor &styler);
    int GetLengthNextNumber(CharacterSet &characterSet, StyleContext &context, LexAccessor &styler);
    int GetLengthNextCharacter(CharacterSet &characterSet, StyleContext &context, LexAccessor &styler);
    int GetLengthNextIdentifier(StyleContext& context, LexAccessor& styler);

public:
	LexerO2M():
        setNumber(CharacterSet::setDigits, ".+-ABCDEFH"),
        setCharacter(CharacterSet::setDigits, "ABCDEFX") {
    }

	virtual ~LexerO2M() {}

	int SCI_METHOD Version() const {
		return lvOriginal;
	}

	void SCI_METHOD Release() {
		delete this;
	}

	const char *SCI_METHOD PropertyNames() {
		return nullptr;
	}

	int SCI_METHOD PropertyType(const char *name) {
		return -1;
	}

	const char *SCI_METHOD DescribeProperty(const char *name) {
		return nullptr;
	}

	int SCI_METHOD PropertySet(const char *key, const char *val) {
		return -1;
	}

	const char *SCI_METHOD DescribeWordListSets() {
		return nullptr;
	}

	int SCI_METHOD WordListSet(int n, const char *wl);

	void SCI_METHOD Lex(unsigned int startPos, int lengthDoc, int initStyle, IDocument *pAccess);
	
	void SCI_METHOD Fold(unsigned int startPos, int lengthDoc, int initStyle, IDocument *pAccess) {}
	
	void *SCI_METHOD PrivateCall(int opration, void *pointer) {
		return nullptr;
	}

	static ILexer *LexerFactoryO2M() {
        return new LexerO2M();
	}
};

int LexerO2M::GetLengthNextWordIfInList(WordList &keywordList, StyleContext &context, LexAccessor &styler) {
	char word[16];
	Sci_Position currPos = (Sci_Position) context.currentPos;
	int i = 0;
	while (i < 15) {
		char ch = styler.SafeGetCharAt(currPos + i);
		if (!IsUpperCase(ch)) {
			break;
		}
		word[i] = ch;
		i++;
	}
	word[i] = '\0';
	if (keywordList.InList(word)) {
		return i;
	}
	else {
		return 0;
	}
}

int LexerO2M::GetLengthNextNumber(CharacterSet &characterSet, StyleContext &context, LexAccessor &styler) {
    std::regex hexadecimalNumber("^\\d[0-9A-F]*H$");
    std::regex integerNumber("^\\d+$");
    std::regex realNumber("^\\d+\\.\\d+([ED][\\+-]\\d+)?$");
    char word[256];
    Sci_Position currPos = (Sci_Position)context.currentPos;
    int i = 0;
    while (i < 255) {
        char ch = styler.SafeGetCharAt(currPos + i);
        if (!characterSet.Contains(ch)) {
            break;
        }
        word[i] = ch;
        i++;
    }
    word[i] = '\0';
    if (std::regex_match(word, hexadecimalNumber) ||
        std::regex_match(word, realNumber) ||
        std::regex_match(word, integerNumber)) {
        return i;
    }
    else {
        return 0;
    }
}

int LexerO2M::GetLengthNextCharacter(CharacterSet &characterSet, StyleContext &context, LexAccessor &styler) {
    std::regex character("^\\d[0-9A-F]{0,2}X$");
    char word[256];
    Sci_Position currPos = (Sci_Position)context.currentPos;
    int i = 0;
    while (i < 255) {
        char ch = styler.SafeGetCharAt(currPos + i);
        if (!characterSet.Contains(ch)) {
            break;
        }
        word[i] = ch;
        i++;
    }
    word[i] = '\0';
    if (std::regex_match(word, character)) {
        return i;
    }
    else {
        return 0;
    }
}

int LexerO2M::GetLengthNextIdentifier(StyleContext& context, LexAccessor& styler) {
    int i = 0;
    Sci_Position currPos = (Sci_Position)context.currentPos;
    char ch = context.ch;

    while (IsAlphaNumeric(ch)) { // find end of identifier
        ++i;
        ch = styler.SafeGetCharAt(currPos + i);
    }

    return i;
}

Sci_Position SCI_METHOD LexerO2M::WordListSet(int n, const char* wl) {
	WordList *wordList = nullptr;
	wordList = &keywords0;
	Sci_Position firstModification = -1;
	if (wordList != nullptr) {
		WordList wlNew;
		wlNew.Set(wl);
		if (*wordList != wlNew) {
			wordList->Set(wl);
			firstModification = 0;
		}
	}
	return firstModification;
}

void SCI_METHOD LexerO2M::Lex(unsigned int startPos, int lengthDoc, int initStyle, IDocument *pAccess) {
	LexAccessor styler(pAccess);
	StyleContext context(startPos, lengthDoc, initStyle, styler);

	for(; context.More(); context.Forward()) {
		if (context.state == SCE_O2M_DEFAULT) {
			if (context.Match("(*")) {
					context.SetState(SCE_O2M_BLOCKCOMMENT);
					context.Forward(); // Move over the '*'
			} else if (context.Match("//")) {
				context.SetState(SCE_O2M_LINECOMMENT);
				context.Forward(); // Move over the 2nd '/'
			} else if (context.Match("'")) {
				context.SetState(SCE_O2M_SINGLESTRING);
            } else if (context.Match('"')) {
                context.SetState(SCE_O2M_DOUBLESTRING);
            } else if (IsADigit(context.ch)) {
                int len;
                if (len = GetLengthNextCharacter(setCharacter, context, styler)) {
                    context.SetState(SCE_O2M_CHARACTER);
                    context.Forward(len - 1);
                } else if (len = GetLengthNextNumber(setNumber, context, styler)) {
                    context.SetState(SCE_O2M_NUMBER);
                    context.Forward(len - 1);
                }
			} else if (IsUpperCase(context.ch)) {
				int len = GetLengthNextWordIfInList(keywords0, context, styler);
				if (len) {
					context.SetState(SCE_O2M_KEYWORD);
					context.Forward(len - 1);
				}
			} else if ((context.ch >= 'a') && (context.ch <= 'z') ||
                (context.ch >= 'A') && (context.ch <= 'Z')) {
                int len = GetLengthNextIdentifier(context, styler);
                if (len) {
                    context.Forward(len - 1);
                }
            }
		}
		else {
			switch (context.state) {
				case SCE_O2M_BLOCKCOMMENT:
					if (context.Match("*)")) {
						context.Forward(); // Move over the ')'
						context.ForwardSetState(SCE_O2M_DEFAULT);
					}
					break;
				case SCE_O2M_LINECOMMENT:
					if (context.atLineEnd) {
                        context.SetState(SCE_O2M_DEFAULT);
					}
					break;
				case SCE_O2M_SINGLESTRING:
					if (context.atLineEnd) {
                        context.SetState(SCE_O2M_DEFAULT);
					} else if (context.Match("'")) {
                        context.ForwardSetState(SCE_O2M_DEFAULT);
					}
					break;
				case SCE_O2M_DOUBLESTRING:
					if (context.atLineEnd) {
                        context.SetState(SCE_O2M_DEFAULT);
					} else if (context.Match('"')) {
                        context.ForwardSetState(SCE_O2M_DEFAULT);
					}
					break;
                case SCE_O2M_NUMBER:
                    context.SetState(SCE_O2M_DEFAULT);
                    break;
                case SCE_O2M_CHARACTER:
                    context.SetState(SCE_O2M_DEFAULT);
                    break;
				case SCE_O2M_KEYWORD:
					context.SetState(SCE_O2M_DEFAULT);
					break;
			}
		}
	}
	context.Complete();
}

LexerModule lmO2M(SCLEX_O2M, LexerO2M::LexerFactoryO2M, "o2m", O2MWordListDesc);
