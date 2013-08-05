//
//
// Derived from Microsoft Sample IME by Jeremy '13,7,17
//
//

#include <map>
#include "Private.h"
#include "resource.h"
#include "BaseWindow.h"
#include "define.h"
#include "TSFDayiBaseStructure.h"
using std::map;

namespace Global {
HINSTANCE dllInstanceHandle;

LONG dllRefCount = -1;

map <WCHAR, WCHAR> radicalMap;

BOOL isWindows8 = FALSE;


BOOL hasPhraseSection = FALSE;
BOOL hasCINPhraseSection = FALSE;

CRITICAL_SECTION CS;
HFONT defaultlFontHandle;				// Global font object we use everywhere


//---------------------------------------------------------------------
// TSFDayi CLSID
//---------------------------------------------------------------------
// {1DE68A87-FF3B-46A0-8F80-46730B2491B1}
extern const CLSID TSFDayiCLSID = 
{ 0x1de68a87, 0xff3b, 0x46a0, { 0x8f, 0x80, 0x46, 0x73, 0xb, 0x24, 0x91, 0xb1 } };


//---------------------------------------------------------------------
// Profile GUID
//---------------------------------------------------------------------
// {36851834-92AD-4397-9F50-800384D5C24C}
extern const GUID TSFDayiGuidProfile = 
{ 0x36851834, 0x92ad, 0x4397, { 0x9f, 0x50, 0x80, 0x3, 0x84, 0xd5, 0xc2, 0x4c } };


//---------------------------------------------------------------------
// PreserveKey GUID
//---------------------------------------------------------------------
// {3C4173E2-D908-419B-BEE0-89FDF46456A2}
extern const GUID TSFDayiGuidImeModePreserveKey = 
{ 0x3c4173e2, 0xd908, 0x419b, { 0xbe, 0xe0, 0x89, 0xfd, 0xf4, 0x64, 0x56, 0xa2 } };

// {B9F1B6BA-CCCF-478F-B7C2-89B0D9442A38}
extern const GUID TSFDayiGuidDoubleSingleBytePreserveKey = 
{ 0xb9f1b6ba, 0xcccf, 0x478f, { 0xb7, 0xc2, 0x89, 0xb0, 0xd9, 0x44, 0x2a, 0x38 } };

//---------------------------------------------------------------------
// Compartments
//---------------------------------------------------------------------
// {91FCB13F-0BA5-4D93-846C-E8A706BB5F2B}
extern const GUID TSFDayiGuidCompartmentIMEMode = 
{ 0x91fcb13f, 0xba5, 0x4d93, { 0x84, 0x6c, 0xe8, 0xa7, 0x6, 0xbb, 0x5f, 0x2b } };
// {176AF217-E72C-4A24-8373-F90581786A5D}
extern const GUID TSFDayiGuidCompartmentDoubleSingleByte = 
{ 0x176af217, 0xe72c, 0x4a24, { 0x83, 0x73, 0xf9, 0x5, 0x81, 0x78, 0x6a, 0x5d } };


//---------------------------------------------------------------------
// LanguageBars
//---------------------------------------------------------------------


// {B9F1B6BA-CCCF-478F-B7C2-89B0D9442A38}
extern const GUID TSFDayiGuidLangBarIMEMode = 
{ 0xb9f1b6ba, 0xcccf, 0x478f, { 0xb7, 0xc2, 0x89, 0xb0, 0xd9, 0x44, 0x2a, 0x38 } };

// {FB007925-ACBE-4AF1-A98B-2701EB68A90B}
extern const GUID TSFDayiGuidLangBarDoubleSingleByte = 
{ 0xfb007925, 0xacbe, 0x4af1, { 0xa9, 0x8b, 0x27, 0x1, 0xeb, 0x68, 0xa9, 0xb } };

// {C1AD6968-5F10-4C73-9125-51A3C70F88A0}
extern const GUID TSFDayiGuidDisplayAttributeInput = 
{ 0xc1ad6968, 0x5f10, 0x4c73, { 0x91, 0x25, 0x51, 0xa3, 0xc7, 0xf, 0x88, 0xa0 } };


extern const GUID TSFDayiGuidDisplayAttributeConverted = 
{ 0x62a68877, 0xc2c1, 0x41ed, { 0x9f, 0x80, 0x54, 0xde, 0xbd, 0x87, 0x9f, 0x74 } };


//---------------------------------------------------------------------
// UI element
//---------------------------------------------------------------------

// {87D9FBA0-C152-475B-BD82-0A18DFA616A7}
extern const GUID TSFDayiGuidCandUIElement = 
{ 0x87d9fba0, 0xc152, 0x475b, { 0xbd, 0x82, 0xa, 0x18, 0xdf, 0xa6, 0x16, 0xa7 } };


//---------------------------------------------------------------------
// Unicode byte order mark
//---------------------------------------------------------------------
extern const WCHAR UnicodeByteOrderMark = 0xFEFF;

//---------------------------------------------------------------------
// dictionary table delimiter
//---------------------------------------------------------------------
extern WCHAR KeywordDelimiter = L'=';
extern const WCHAR StringDelimiter  = L'\"';

//---------------------------------------------------------------------
// defined item in setting file table [PreservedKey] section
//---------------------------------------------------------------------
extern const WCHAR ImeModeDescription[] = L"Chinese/English input (Shift)";
extern const int ImeModeOnIcoIndex = IME_MODE_ON_ICON_INDEX;
extern const int ImeModeOffIcoIndex = IME_MODE_OFF_ICON_INDEX;

extern const WCHAR DoubleSingleByteDescription[] = L"Double/Single byte (Shift+Space)";
extern const int DoubleSingleByteOnIcoIndex = IME_DOUBLE_ON_INDEX;
extern const int DoubleSingleByteOffIcoIndex = IME_DOUBLE_OFF_INDEX;


//---------------------------------------------------------------------
// defined item in setting file table [LanguageBar] section
//---------------------------------------------------------------------
extern const WCHAR LangbarImeModeDescription[] = L"Conversion mode";
extern const WCHAR LangbarDoubleSingleByteDescription[] = L"Character width";


//---------------------------------------------------------------------
// windows class / titile / atom
//---------------------------------------------------------------------
extern const WCHAR CandidateClassName[] = L"TSFDayi.CandidateWindow";
ATOM AtomCandidateWindow;
extern const WCHAR CandidateShadowClassName[] = L"TSFDayi.CandidateShadowWindow";
ATOM AtomCandidateShadowWindow;
extern const WCHAR CandidateScrollBarClassName[] = L"TSFDayi.CandidateScrollBarWindow";
ATOM AtomCandidateScrollBarWindow;
extern const WCHAR NotifyClassName[] = L"TSFDayi.NotifyWindow";
ATOM AtomNotifyWindow;
extern const WCHAR NotifyShadowClassName[] = L"TSFDayi.NotifyShadowWindow";
ATOM AtomNotifyShadowWindow;


BOOL RegisterWindowClass()
{
    if (!CBaseWindow::_InitWindowClass(CandidateClassName, &AtomCandidateWindow))
    {
        return FALSE;
    }
    if (!CBaseWindow::_InitWindowClass(CandidateShadowClassName, &AtomCandidateShadowWindow))
    {
        return FALSE;
    }
    if (!CBaseWindow::_InitWindowClass(CandidateScrollBarClassName, &AtomCandidateScrollBarWindow))
    {
        return FALSE;
    }
	
    if (!CBaseWindow::_InitWindowClass(NotifyClassName, &AtomNotifyWindow))
    {
        return FALSE;
    }
    if (!CBaseWindow::_InitWindowClass(NotifyShadowClassName, &AtomNotifyShadowWindow))
    {
        return FALSE;
    }

    return TRUE;
}

//---------------------------------------------------------------------
// defined full width characters for Double/Single byte conversion
//---------------------------------------------------------------------
extern const WCHAR FullWidthCharTable[] = {
    //         !       "       #       $       %       &       '       (    )       *       +       ,       -       .       /
    0x3000, 0xFF01, 0xFF02, 0xFF03, 0xFF04, 0xFF05, 0xFF06, 0xFF07, 0xFF08, 0xFF09, 0xFF0A, 0xFF0B, 0xFF0C, 0xFF0D, 0xFF0E, 0xFF0F,
    // 0       1       2       3       4       5       6       7       8       9       :       ;       <       =       >       ?
    0xFF10, 0xFF11, 0xFF12, 0xFF13, 0xFF14, 0xFF15, 0xFF16, 0xFF17, 0xFF18, 0xFF19, 0xFF1A, 0xFF1B, 0xFF1C, 0xFF1D, 0xFF1E, 0xFF1F,
    // @       A       B       C       D       E       F       G       H       I       J       K       L       M       N       0
    0xFF20, 0xFF21, 0xFF22, 0xFF23, 0xFF24, 0xFF25, 0xFF26, 0xFF27, 0xFF28, 0xFF29, 0xFF2A, 0xFF2B, 0xFF2C, 0xFF2D, 0xFF2E, 0xFF2F,
    // P       Q       R       S       T       U       V       W       X       Y       Z       [       \       ]       ^       _
    0xFF30, 0xFF31, 0xFF32, 0xFF33, 0xFF34, 0xFF35, 0xFF36, 0xFF37, 0xFF38, 0xFF39, 0xFF3A, 0xFF3B, 0xFF3C, 0xFF3D, 0xFF3E, 0xFF3F,
    // '       a       b       c       d       e       f       g       h       i       j       k       l       m       n       o       
    0xFF40, 0xFF41, 0xFF42, 0xFF43, 0xFF44, 0xFF45, 0xFF46, 0xFF47, 0xFF48, 0xFF49, 0xFF4A, 0xFF4B, 0xFF4C, 0xFF4D, 0xFF4E, 0xFF4F,
    // p       q       r       s       t       u       v       w       x       y       z       {       |       }       ~
    0xFF50, 0xFF51, 0xFF52, 0xFF53, 0xFF54, 0xFF55, 0xFF56, 0xFF57, 0xFF58, 0xFF59, 0xFF5A, 0xFF5B, 0xFF5C, 0xFF5D, 0xFF5E
};

//---------------------------------------------------------------------
// defined symbol characters
//---------------------------------------------------------------------
extern const WCHAR symbolCharTable[26] = {
	L' ', L'!', L'@', L'\\', L'\"',  L'#', L'$', L'%', L'&', L'\'', L'(', L')',	L'+', L':', L'<', L'>', L'[', L']', L'^', L'-', L'_', L'`', L'{', L'}', L'|', L'~'
};
//---------------------------------------------------------------------
// defined directly input address characters
//---------------------------------------------------------------------
extern const _AddressDirectInput addressCharTable[5] = {
	{L'\'', L'��'},
	{L'[', L'��'},
	{L']', L'��'},
	{L'-', L'�m'},
	{L'\\', L'��'}
};


//+---------------------------------------------------------------------------
//
// CheckModifiers
//
//----------------------------------------------------------------------------

#define TF_MOD_ALLALT     (TF_MOD_RALT | TF_MOD_LALT | TF_MOD_ALT)
#define TF_MOD_ALLCONTROL (TF_MOD_RCONTROL | TF_MOD_LCONTROL | TF_MOD_CONTROL)
#define TF_MOD_ALLSHIFT   (TF_MOD_RSHIFT | TF_MOD_LSHIFT | TF_MOD_SHIFT)
#define TF_MOD_RLALT      (TF_MOD_RALT | TF_MOD_LALT)
#define TF_MOD_RLCONTROL  (TF_MOD_RCONTROL | TF_MOD_LCONTROL)
#define TF_MOD_RLSHIFT    (TF_MOD_RSHIFT | TF_MOD_LSHIFT)

#define CheckMod(m0, m1, mod)        \
    if (m1 & TF_MOD_ ## mod ##)      \
{ \
    if (!(m0 & TF_MOD_ ## mod ##)) \
{      \
    return FALSE;   \
}      \
} \
    else       \
{ \
    if ((m1 ^ m0) & TF_MOD_RL ## mod ##)    \
{      \
    return FALSE;   \
}      \
} \



BOOL CheckModifiers(UINT modCurrent, UINT mod)
{
    mod &= ~TF_MOD_ON_KEYUP;

    if (mod & TF_MOD_IGNORE_ALL_MODIFIER)
    {
        return TRUE;
    }

    if (modCurrent == mod)
    {
        return TRUE;
    }

    if (modCurrent && !mod)
    {
        return FALSE;
    }

    CheckMod(modCurrent, mod, ALT);
    CheckMod(modCurrent, mod, SHIFT);
    CheckMod(modCurrent, mod, CONTROL);

    return TRUE;
}

//+---------------------------------------------------------------------------
//
// UpdateModifiers
//
//    wParam - virtual-key code
//    lParam - [0-15]  Repeat count
//  [16-23] Scan code
//  [24]    Extended key
//  [25-28] Reserved
//  [29]    Context code
//  [30]    Previous key state
//  [31]    Transition state
//----------------------------------------------------------------------------

USHORT ModifiersValue = 0;
BOOL   IsShiftKeyDownOnly = FALSE;
BOOL   IsControlKeyDownOnly = FALSE;
BOOL   IsAltKeyDownOnly = FALSE;

BOOL UpdateModifiers(WPARAM wParam, LPARAM lParam)
{
    // high-order bit : key down
    // low-order bit  : toggled
    SHORT sksMenu = GetKeyState(VK_MENU);
    SHORT sksCtrl = GetKeyState(VK_CONTROL);
    SHORT sksShft = GetKeyState(VK_SHIFT);

    switch (wParam & 0xff)
    {
    case VK_MENU:
        // is VK_MENU down?
        if (sksMenu & 0x8000)
        {
            // is extended key?
            if (lParam & 0x01000000)
            {
                ModifiersValue |= (TF_MOD_RALT | TF_MOD_ALT);
            }
            else
            {
                ModifiersValue |= (TF_MOD_LALT | TF_MOD_ALT);
            }

            // is previous key state up?
            if (!(lParam & 0x40000000))
            {
                // is VK_CONTROL and VK_SHIFT up?
                if (!(sksCtrl & 0x8000) && !(sksShft & 0x8000))
                {
                    IsAltKeyDownOnly = TRUE;
                }
                else
                {
                    IsShiftKeyDownOnly = FALSE;
                    IsControlKeyDownOnly = FALSE;
                    IsAltKeyDownOnly = FALSE;
                }
            }
        }
        break;

    case VK_CONTROL:
        // is VK_CONTROL down?
        if (sksCtrl & 0x8000)    
        {
            // is extended key?
            if (lParam & 0x01000000)
            {
                ModifiersValue |= (TF_MOD_RCONTROL | TF_MOD_CONTROL);
            }
            else
            {
                ModifiersValue |= (TF_MOD_LCONTROL | TF_MOD_CONTROL);
            }

            // is previous key state up?
            if (!(lParam & 0x40000000))
            {
                // is VK_SHIFT and VK_MENU up?
                if (!(sksShft & 0x8000) && !(sksMenu & 0x8000))
                {
                    IsControlKeyDownOnly = TRUE;
                }
                else
                {
                    IsShiftKeyDownOnly = FALSE;
                    IsControlKeyDownOnly = FALSE;
                    IsAltKeyDownOnly = FALSE;
                }
            }
        }
        break;

    case VK_SHIFT:
        // is VK_SHIFT down?
        if (sksShft & 0x8000)    
        {
            // is scan code 0x36(right shift)?
            if (((lParam >> 16) & 0x00ff) == 0x36)
            {
                ModifiersValue |= (TF_MOD_RSHIFT | TF_MOD_SHIFT);
            }
            else
            {
                ModifiersValue |= (TF_MOD_LSHIFT | TF_MOD_SHIFT);
            }

            // is previous key state up?
            if (!(lParam & 0x40000000))
            {
                // is VK_MENU and VK_CONTROL up?
                if (!(sksMenu & 0x8000) && !(sksCtrl & 0x8000))
                {
                    IsShiftKeyDownOnly = TRUE;
                }
                else
                {
                    IsShiftKeyDownOnly = FALSE;
                    IsControlKeyDownOnly = FALSE;
                    IsAltKeyDownOnly = FALSE;
                }
            }
        }
        break;

    default:
        IsShiftKeyDownOnly = FALSE;
        IsControlKeyDownOnly = FALSE;
        IsAltKeyDownOnly = FALSE;
        break;
    }

    if (!(sksMenu & 0x8000))
    {
        ModifiersValue &= ~TF_MOD_ALLALT;
    }
    if (!(sksCtrl & 0x8000))
    {
        ModifiersValue &= ~TF_MOD_ALLCONTROL;
    }
    if (!(sksShft & 0x8000))
    {
        ModifiersValue &= ~TF_MOD_ALLSHIFT;
    }

    return TRUE;
}

//---------------------------------------------------------------------
// override CompareElements
//---------------------------------------------------------------------
BOOL CompareElements(LCID locale, const CStringRange* pElement1, const CStringRange* pElement2)
{
    return (CStringRange::Compare(locale, (CStringRange*)pElement1, (CStringRange*)pElement2) == CSTR_EQUAL) ? TRUE : FALSE;
}


}