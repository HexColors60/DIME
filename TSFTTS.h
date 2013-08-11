//
//
// Derived from Microsoft Sample IME by Jeremy '13,7,17
//
//
#ifndef TSFTTS_H
#define TSFTTS_H

#pragma once

#include "KeyHandlerEditSession.h"
#include "BaseStructure.h"
#include "Compartment.h"
#include "LanguageBar.h"


class CLangBarItemButton;
class CUIPresenter;
class CCompositionProcessorEngine;

const DWORD WM_CheckGlobalCompartment = WM_USER;
LRESULT CALLBACK CTSFTTS_WindowProc(HWND wndHandle, UINT uMsg, WPARAM wParam, LPARAM lParam);

class CTSFTTS : public ITfTextInputProcessorEx,
    public ITfThreadMgrEventSink,
    public ITfTextEditSink,
    public ITfKeyEventSink,
    public ITfCompositionSink,
    public ITfDisplayAttributeProvider,
    public ITfActiveLanguageProfileNotifySink,
    public ITfThreadFocusSink,
    public ITfFunctionProvider,
    public ITfFnGetPreferredTouchKeyboardLayout,
	public ITfFnConfigure,//control panel application
	public ITfFnShowHelp
{
public:
    CTSFTTS();
    ~CTSFTTS();

    // IUnknown
    STDMETHODIMP QueryInterface(REFIID riid, _Outptr_ void **ppvObj);
    STDMETHODIMP_(ULONG) AddRef(void);
    STDMETHODIMP_(ULONG) Release(void);

    // ITfTextInputProcessor
    STDMETHODIMP Activate(ITfThreadMgr *pThreadMgr, TfClientId tfClientId) {
        return ActivateEx(pThreadMgr, tfClientId, 0);
    }
    // ITfTextInputProcessorEx
    STDMETHODIMP ActivateEx(ITfThreadMgr *pThreadMgr, TfClientId tfClientId, DWORD dwFlags);
    STDMETHODIMP Deactivate();

    // ITfThreadMgrEventSink
    STDMETHODIMP OnInitDocumentMgr(_In_ ITfDocumentMgr *pDocMgr);
    STDMETHODIMP OnUninitDocumentMgr(_In_ ITfDocumentMgr *pDocMgr);
    STDMETHODIMP OnSetFocus(_In_ ITfDocumentMgr *pDocMgrFocus, _In_ ITfDocumentMgr *pDocMgrPrevFocus);
    STDMETHODIMP OnPushContext(_In_ ITfContext *pContext);
    STDMETHODIMP OnPopContext(_In_ ITfContext *pContext);

    // ITfTextEditSink
    STDMETHODIMP OnEndEdit(__RPC__in_opt ITfContext *pContext, TfEditCookie ecReadOnly, __RPC__in_opt ITfEditRecord *pEditRecord);

    // ITfKeyEventSink
    STDMETHODIMP OnSetFocus(BOOL fForeground);
    STDMETHODIMP OnTestKeyDown(ITfContext *pContext, WPARAM wParam, LPARAM lParam, BOOL *pIsEaten);
    STDMETHODIMP OnKeyDown(ITfContext *pContext, WPARAM wParam, LPARAM lParam, BOOL *pIsEaten);
    STDMETHODIMP OnTestKeyUp(ITfContext *pContext, WPARAM wParam, LPARAM lParam, BOOL *pIsEaten);
    STDMETHODIMP OnKeyUp(ITfContext *pContext, WPARAM wParam, LPARAM lParam, BOOL *pIsEaten);
    STDMETHODIMP OnPreservedKey(ITfContext *pContext, REFGUID rguid, BOOL *pIsEaten);

    // ITfCompositionSink
    STDMETHODIMP OnCompositionTerminated(TfEditCookie ecWrite, _In_ ITfComposition *pComposition);

    // ITfDisplayAttributeProvider
    STDMETHODIMP EnumDisplayAttributeInfo(__RPC__deref_out_opt IEnumTfDisplayAttributeInfo **ppEnum);
    STDMETHODIMP GetDisplayAttributeInfo(__RPC__in REFGUID guidInfo, __RPC__deref_out_opt ITfDisplayAttributeInfo **ppInfo);

    // ITfActiveLanguageProfileNotifySink
    STDMETHODIMP OnActivated(_In_ REFCLSID clsid, _In_ REFGUID guidProfile, _In_ BOOL isActivated);

    // ITfThreadFocusSink
    STDMETHODIMP OnSetThreadFocus();
    STDMETHODIMP OnKillThreadFocus();

    // ITfFunctionProvider
    STDMETHODIMP GetType(__RPC__out GUID *pguid);
    STDMETHODIMP GetDescription(__RPC__deref_out_opt BSTR *pbstrDesc);
    STDMETHODIMP GetFunction(__RPC__in REFGUID rguid, __RPC__in REFIID riid, __RPC__deref_out_opt IUnknown **ppunk);

    // ITfFunction
    STDMETHODIMP GetDisplayName(_Out_ BSTR *pbstrDisplayName);

    // ITfFnGetPreferredTouchKeyboardLayout, it is the Optimized layout feature.
    STDMETHODIMP GetLayout(_Out_ TKBLayoutType *ptkblayoutType, _Out_ WORD *pwPreferredLayoutId);

	//ITfFnConfigure 
	STDMETHODIMP Show(_In_ HWND hwndParent, _In_ LANGID langid, _In_ REFGUID rguidProfile);
	// ITfFnShowHelp
    STDMETHODIMP Show(_In_ HWND hwndParent);

	 // Get language profile.
    //GUID GetLanguageProfile(LANGID *plangid){ *plangid = _langid;  return _guidProfile;}
    // Get locale
    LCID GetLocale(){return MAKELCID(_langid, SORT_DEFAULT);}
    

    // CClassFactory factory callback
    static HRESULT CreateInstance(_In_ IUnknown *pUnkOuter, REFIID riid, _Outptr_ void **ppvObj);

    // utility function for thread manager.
    ITfThreadMgr* _GetThreadMgr() { return _pThreadMgr; }
    TfClientId _GetClientId() { return _tfClientId; }

    // functions for the composition object.
    void _SetComposition(_In_ ITfComposition *pComposition);
    void _TerminateComposition(TfEditCookie ec, _In_ ITfContext *pContext, BOOL isCalledFromDeactivate = FALSE);
    void _SaveCompositionContext(_In_ ITfContext *pContext);

    // key event handlers for composition/candidate/phrase common objects.
    HRESULT _HandleComplete(TfEditCookie ec, _In_ ITfContext *pContext);
    HRESULT _HandleCancel(TfEditCookie ec, _In_ ITfContext *pContext);

    // key event handlers for composition object.
    HRESULT _HandleCompositionInput(TfEditCookie ec, _In_ ITfContext *pContext, WCHAR wch);
    HRESULT _HandleCompositionFinalize(TfEditCookie ec, _In_ ITfContext *pContext, BOOL fCandidateList);
    HRESULT _HandleCompositionConvert(TfEditCookie ec, _In_ ITfContext *pContext, BOOL isWildcardSearch);
    HRESULT _HandleCompositionBackspace(TfEditCookie ec, _In_ ITfContext *pContext);
    HRESULT _HandleCompositionArrowKey(TfEditCookie ec, _In_ ITfContext *pContext, KEYSTROKE_FUNCTION keyFunction);
    HRESULT _HandleCompositionDoubleSingleByte(TfEditCookie ec, _In_ ITfContext *pContext, WCHAR wch);
	HRESULT _HandleCompositionAddressChar(TfEditCookie ec, _In_ ITfContext *pContext, WCHAR wch);
	// function for textlayoutchange.
	HRESULT _HandlTextLayoutChange(TfEditCookie ec, _In_ ITfContext *pContext, _In_ ITfRange *pRangeComposition);

    // key event handlers for candidate object.
    HRESULT _HandleCandidateFinalize(TfEditCookie ec, _In_ ITfContext *pContext);
    HRESULT _HandleCandidateConvert(TfEditCookie ec, _In_ ITfContext *pContext);
    HRESULT _HandleCandidateArrowKey(TfEditCookie ec, _In_ ITfContext *pContext, _In_ KEYSTROKE_FUNCTION keyFunction);
    HRESULT _HandleCandidateSelectByNumber(TfEditCookie ec, _In_ ITfContext *pContext, _In_ UINT uCode);

    // key event handlers for phrase object.
    HRESULT _HandlePhraseFinalize(TfEditCookie ec, _In_ ITfContext *pContext);
    HRESULT _HandlePhraseArrowKey(TfEditCookie ec, _In_ ITfContext *pContext, _In_ KEYSTROKE_FUNCTION keyFunction);
    HRESULT _HandlePhraseSelectByNumber(TfEditCookie ec, _In_ ITfContext *pContext, _In_ UINT uCode);

    BOOL _IsSecureMode(void) { return (_dwActivateFlags & TF_TMAE_SECUREMODE) ? TRUE : FALSE; }
    BOOL _IsComLess(void) { return (_dwActivateFlags & TF_TMAE_COMLESS) ? TRUE : FALSE; }
	BOOL _IsStoreAppMode(void) { return (_dwActivateFlags & TF_TMF_IMMERSIVEMODE) ? TRUE : FALSE; }
	BOOL _IsUILessMode(void);

    CCompositionProcessorEngine* GetCompositionProcessorEngine() { return (_pCompositionProcessorEngine); }

    // comless helpers
    static HRESULT CreateInstance(REFCLSID rclsid, REFIID riid, _Outptr_result_maybenull_ LPVOID* ppv, _Out_opt_ HINSTANCE* phInst, BOOL isComLessMode);
    static HRESULT ComLessCreateInstance(REFGUID rclsid, REFIID riid, _Outptr_result_maybenull_ void **ppv, _Out_opt_ HINSTANCE *phInst);
    static HRESULT GetComModuleName(REFGUID rclsid, _Out_writes_(cchPath)WCHAR* wchPath, DWORD cchPath);
	
	
	//Called when compartment status changed.
	void OnKeyboardClosed();
	void OnKeyboardOpen();
	void OnSwitchedToFullShape();
	void OnSwitchedToHalfShape();

	HRESULT ShowNotifyText(CStringRange *pNotifyText);


	//configuration propertysheet dialog
	static INT_PTR CALLBACK CTSFTTS::CommonPropertyPageWndProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
	friend void DrawColor(HWND hwnd, HDC hdc, COLORREF col);
	
	//  configuration set/get
	static void SetAutoCompose(BOOL autoCompose) {_autoCompose = autoCompose;}
	static BOOL GetAutoCompose() {return _autoCompose;}
	static void SetThreeCodeMode(BOOL threeCodeMode) {_threeCodeMode = threeCodeMode;}
	static BOOL GetThreeCodeMode() {return _threeCodeMode;}
	static void SetFontSize(UINT fontSize) {_fontSize = fontSize;}
	static UINT GetFontSize() {return _fontSize;}
	static void SetFontWeight(UINT fontWeight) {_fontWeight = fontWeight;}
	static UINT GetFontWeight() {return _fontWeight;}
	static BOOL GetFontItalic() {return _fontItalic;}
	static void SetFontItalic(BOOL fontItalic) {_fontItalic = fontItalic;}
	static void SetMaxCodes(UINT maxCodes) { _maxCodes = maxCodes;}
	static UINT GetMaxCodes(){return _maxCodes;}
	static void SetDoBeep(BOOL doBeep) { _doBeep = doBeep;}
	static BOOL GetDoBeep() {return _doBeep;}
	static void SetMakePhrase(BOOL makePhrase) { _makePhrase = makePhrase;}
	static BOOL GetMakePhrase() {return _makePhrase;}
	static void SetFontFaceName(WCHAR *pFontFaceName) {_pFontFaceName = pFontFaceName;}
	static WCHAR* GetFontFaceName(){ return _pFontFaceName;}
	//colors
	static void SetItemColor(UINT itemColor) { _itemColor = itemColor;}
	static COLORREF GetItemColor(){return _itemColor;}
	static void SetPhraseColor(UINT phraseColor) { _phraseColor = phraseColor;}
	static COLORREF GetPhraseColor(){return _phraseColor;}
	static void SetNumberColor(UINT numberColor) { _numberColor = numberColor;}
	static COLORREF GetNumberColor(){return _numberColor;}
	static void SetItemBGColor(UINT itemBGColor) { _itemBGColor = itemBGColor;}
	static COLORREF GetItemBGColor(){return _itemBGColor;}
	static void SetSelectedColor(UINT selectedColor) { _selectedColor = selectedColor;}
	static COLORREF GetSelectedColor(){return _selectedColor;}
	static void SetSelectedBGColor(UINT selectedBGColor) { _selectedBGColor = selectedBGColor;}
	static COLORREF GetSelectedBGColor(){return _selectedBGColor;}
	
	
	static void SetActivatedKeyboardMode(BOOL activatedKeyboardMode) { _activatedKeyboardMode = activatedKeyboardMode;}
	static BOOL GetActivatedKeyboardMode() {return _activatedKeyboardMode;}
	static void SetAppPermissionSet(BOOL appPermissionSet) { _appPermissionSet = appPermissionSet;}
	static BOOL GetAppPermissionSet() {return _appPermissionSet;}
	
	static VOID WriteConfig();
	VOID LoadConfig();
private:
    // functions for the composition object.
    HRESULT _HandleCompositionInputWorker(_In_ CCompositionProcessorEngine *pCompositionProcessorEngine, TfEditCookie ec, _In_ ITfContext *pContext);
    HRESULT _CreateAndStartCandidate(_In_ CCompositionProcessorEngine *pCompositionProcessorEngine, TfEditCookie ec, _In_ ITfContext *pContext);
    HRESULT _HandleCandidateWorker(TfEditCookie ec, _In_ ITfContext *pContext);

    void _StartComposition(_In_ ITfContext *pContext);
    void _EndComposition(_In_opt_ ITfContext *pContext);
    BOOL _IsComposing();
    BOOL _IsKeyboardDisabled();

    HRESULT _AddComposingAndChar(TfEditCookie ec, _In_ ITfContext *pContext, _In_ CStringRange *pstrAddString);
    HRESULT _AddCharAndFinalize(TfEditCookie ec, _In_ ITfContext *pContext, _In_ CStringRange *pstrAddString);

    BOOL _FindComposingRange(TfEditCookie ec, _In_ ITfContext *pContext, _In_ ITfRange *pSelection, _Outptr_result_maybenull_ ITfRange **ppRange);
    HRESULT _SetInputString(TfEditCookie ec, _In_ ITfContext *pContext, _Out_opt_ ITfRange *pRange, _In_ CStringRange *pstrAddString, BOOL exist_composing);
    HRESULT _InsertAtSelection(TfEditCookie ec, _In_ ITfContext *pContext, _In_ CStringRange *pstrAddString, _Outptr_ ITfRange **ppCompRange);

    HRESULT _RemoveDummyCompositionForComposing(TfEditCookie ec, _In_ ITfComposition *pComposition);

    // Invoke key handler edit session
    HRESULT _InvokeKeyHandler(_In_ ITfContext *pContext, UINT code, WCHAR wch, DWORD flags, _KEYSTROKE_STATE keyState);

    // function for the language property
    BOOL _SetCompositionLanguage(TfEditCookie ec, _In_ ITfContext *pContext);

    // function for the display attribute
    void _ClearCompositionDisplayAttributes(TfEditCookie ec, _In_ ITfContext *pContext);
    BOOL _SetCompositionDisplayAttributes(TfEditCookie ec, _In_ ITfContext *pContext, TfGuidAtom gaDisplayAttribute);
    BOOL _InitDisplayAttributeGuidAtom();

    BOOL _InitThreadMgrEventSink();
    void _UninitThreadMgrEventSink();

    BOOL _InitTextEditSink(_In_ ITfDocumentMgr *pDocMgr);

    void _UpdateLanguageBarOnSetFocus(_In_ ITfDocumentMgr *pDocMgrFocus);

    BOOL _InitKeyEventSink();
    void _UninitKeyEventSink();

    BOOL _InitActiveLanguageProfileNotifySink();
    void _UninitActiveLanguageProfileNotifySink();

    BOOL _IsKeyEaten(_In_ ITfContext *pContext, UINT codeIn, _Out_ UINT *pCodeOut, _Out_writes_(1) WCHAR *pwch, _Out_opt_ _KEYSTROKE_STATE *pKeyState);

    BOOL _IsRangeCovered(TfEditCookie ec, _In_ ITfRange *pRangeTest, _In_ ITfRange *pRangeCover);
   

    WCHAR ConvertVKey(UINT code);

    BOOL _InitThreadFocusSink();
    void _UninitThreadFocusSink();

    BOOL _InitFunctionProviderSink();
    void _UninitFunctionProviderSink();

    BOOL _AddTextProcessorEngine();

    BOOL VerifyTSFTTSCLSID(_In_ REFCLSID clsid);

    friend LRESULT CALLBACK CTSFTTS_WindowProc(HWND wndHandle, UINT uMsg, WPARAM wParam, LPARAM lParam);

	// function for process candidate
	VOID _DeleteCandidateList(BOOL fForce, _In_opt_ ITfContext *pContext);

	//language bar private
    BOOL InitLanguageBar(_In_ CLangBarItemButton *pLanguageBar, _In_ ITfThreadMgr *pThreadMgr, TfClientId tfClientId, REFGUID guidCompartment);
    void SetupLanguageBar(_In_ ITfThreadMgr *pThreadMgr, TfClientId tfClientId, BOOL isSecureMode);
	void InitializeTSFTTSCompartment(_In_ ITfThreadMgr *pThreadMgr, TfClientId tfClientId);
    void CreateLanguageBarButton(DWORD dwEnable, GUID guidLangBar, _In_z_ LPCWSTR pwszDescriptionValue, _In_z_ LPCWSTR pwszTooltipValue, DWORD dwOnIconIndex, DWORD dwOffIconIndex, _Outptr_result_maybenull_ CLangBarItemButton **ppLangBarItemButton, BOOL isSecureMode);
    static HRESULT CompartmentCallback(_In_ void *pv, REFGUID guidCompartment);
    void PrivateCompartmentsUpdated(_In_ ITfThreadMgr *pThreadMgr);
    void KeyboardOpenCompartmentUpdated(_In_ ITfThreadMgr *pThreadMgr, _In_ REFGUID guidCompartment);
	// Language bar control
    BOOL SetupLanguageProfile(LANGID langid, REFGUID guidLanguageProfile, _In_ ITfThreadMgr *pThreadMgr, TfClientId tfClientId, BOOL isSecureMode);
    void SetLanguageBarStatus(DWORD status, BOOL isSet);
    void ConversionModeCompartmentUpdated(_In_ ITfThreadMgr *pThreadMgr, BOOL *setKeyboardOpenClose = NULL);
    void ShowAllLanguageBarIcons();
    void HideAllLanguageBarIcons();




	void SetDefaultTextFont();

private:
	
	LANGID _langid;
    GUID _guidProfile;
	// Language bar data
    CLangBarItemButton* _pLanguageBar_IMEModeW8;
	CLangBarItemButton* _pLanguageBar_IMEMode;
    CLangBarItemButton* _pLanguageBar_DoubleSingleByte;

    // Compartment
    CCompartment* _pCompartmentConversion;
	CCompartmentEventSink* _pCompartmentIMEModeEventSink;
    CCompartmentEventSink* _pCompartmentConversionEventSink;
    CCompartmentEventSink* _pCompartmentKeyboardOpenEventSink;
    CCompartmentEventSink* _pCompartmentDoubleSingleByteEventSink;


    ITfThreadMgr* _pThreadMgr;
    TfClientId _tfClientId;
    DWORD _dwActivateFlags;

    // The cookie of ThreadMgrEventSink
    DWORD _threadMgrEventSinkCookie;

    ITfContext* _pTextEditSinkContext;
    DWORD _textEditSinkCookie;

    // The cookie of ActiveLanguageProfileNotifySink
    DWORD _activeLanguageProfileNotifySinkCookie;

    // The cookie of ThreadFocusSink
    DWORD _dwThreadFocusSinkCookie;

    // Composition Processor Engine object.
    CCompositionProcessorEngine* _pCompositionProcessorEngine;

    // Language bar item object.
    CLangBarItemButton* _pLangBarItem;

    // the current composition object.
    ITfComposition* _pComposition;

    // guidatom for the display attibute.
    TfGuidAtom _gaDisplayAttributeInput;
    TfGuidAtom _gaDisplayAttributeConverted;

    CANDIDATE_MODE _candidateMode;
    CUIPresenter *_pUIPresenter;
    BOOL _isCandidateWithWildcard : 1;

    ITfDocumentMgr* _pDocMgrLastFocused;

    ITfContext* _pContext;

    ITfCompartment* _pSIPIMEOnOffCompartment;
    DWORD _dwSIPIMEOnOffCompartmentSinkCookie;

    HWND _msgWndHandle; 

    LONG _refCount;

    // Support the search integration
    ITfFnSearchCandidateProvider* _pITfFnSearchCandidateProvider;

	//for phrase cand killing when cursor moved
	BOOL _phraseCandShowing;
	POINT _phraseCandLocation;
	
	
	//user setting variables
	static BOOL _autoCompose;
	static BOOL _threeCodeMode;
	static BOOL _doBeep;
	static BOOL _appPermissionSet;
	static BOOL _activatedKeyboardMode;
	static BOOL _makePhrase;
    static UINT _fontSize;
	static UINT _fontWeight;
	static BOOL _fontItalic;
	static UINT _maxCodes;
	static WCHAR* _pFontFaceName;
	static COLORREF _itemColor;
	static COLORREF _phraseColor;
	static COLORREF _numberColor;
	static COLORREF _itemBGColor;
	static COLORREF _selectedColor;
	static COLORREF _selectedBGColor;
};


#endif
