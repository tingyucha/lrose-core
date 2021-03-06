
//////////////////////////////////////////////////////////////
//
// Header file for RpmainformClass
//
//    This file is generated by RapidApp 1.2
//
//    This class implements the user interface portion of a class
//
//    If you do modify this class, limit your changes to adding
//    members below the "//--- End generated code section" markers
//
//    This will allow RapidApp to integrate changes more easily
//
//    This class is a ViewKit user interface "component".
//    For more information on how components are used, see the
//    "ViewKit Programmers' Manual", and the RapidApp
//    User's Guide.
//
//////////////////////////////////////////////////////////////
#ifndef RPMAINFORMCLASS_H
#define RPMAINFORMCLASS_H
#include <Vk/VkComponent.h>

#include <Vk/VkWindow.h>
#include <Vk/VkMenuBar.h>
#include <Vk/VkSubMenu.h>
//---- End generated headers

#include "RpFileSelectionForm.h"

// Externally defined classes referenced by this class:

class RpTabbedDeckClass;
class RpmainSeqFormClass;

extern Widget SaveDispLayoutWidget;
extern Widget LoadDispLayoutWidget;

class RpmainformClass : public VkComponent
{

  public:

    RpmainformClass ( const char *, Widget );
    RpmainformClass ( const char * );
    ~RpmainformClass();
    void create ( Widget );
    const char *  className();
    virtual void setParent(class VkWindow *);
    virtual void AllowReplayChanged(Widget, XtPointer);
    virtual void ackAllAlerts(Widget, XtPointer);
    virtual void openTestSat(Widget, XtPointer);
    virtual void setWinGroupsToggle1(Widget, XtPointer);
    virtual void setWinGroupsToggle2(Widget, XtPointer);
    virtual void setWinGroupsToggle3(Widget, XtPointer);
    virtual void setWinGroupsToggle4(Widget, XtPointer);
    virtual void setWinGroupsToggle5(Widget, XtPointer);
    virtual void setWinGroupsToggle6(Widget, XtPointer);
    virtual void setWinGroupsToggle7(Widget, XtPointer);
    virtual void setWinGroupsToggle8(Widget, XtPointer);
    virtual void setWinGroupsToggle9(Widget, XtPointer);
    virtual void setWinGroupsToggleAll(Widget, XtPointer);
    virtual void silenceAllAlertsChanged(Widget, XtPointer);
    virtual void suppressAllAlertsChanged(Widget, XtPointer);
    virtual void suppressFutureChanged(Widget, XtPointer);
    virtual void uifDelAllSequence(Widget, XtPointer);
    virtual void uifExitRequest(Widget, XtPointer);
    virtual void uifOpenAccumMng(Widget, XtPointer);
    virtual void uifOpenNewCommMng(Widget, XtPointer);
    virtual void uifSaveSched(Widget, XtPointer);
    virtual void uifSetZRCoeff(Widget, XtPointer);
    virtual void uifaddCAPPI(Widget, XtPointer);
    virtual void uifaddPPI(Widget, XtPointer);
    virtual void uifaddPPIVX(Widget, XtPointer);
    virtual void uifaddPPIVX3D(Widget, XtPointer);
    virtual void uifaddTops(Widget, XtPointer);
    virtual void uifaddVIL(Widget, XtPointer);
    virtual void uifdeleteImg(Widget, XtPointer);
    virtual void uiflatestComplete(Widget, XtPointer);
    virtual void uiflatestScanbyScan(Widget, XtPointer);
    virtual void uiflatestStatic(Widget, XtPointer);
    virtual void uifloadRealTime(Widget, XtPointer);
    virtual void uifloadcmapDefault(Widget, XtPointer);
    virtual void uifloadcmapPrinter(Widget, XtPointer);
    virtual void uifloadlayoutDefault(Widget, XtPointer);
    virtual void uifloadlayoutNamed(Widget, XtPointer);
    virtual void uiflocalTimeChanged(Widget, XtPointer);
    virtual void uifopenDBBrowser(Widget, XtPointer);
    virtual void uifopenEditReq(Widget, XtPointer);
    virtual void uifopenEditSched(Widget, XtPointer);
    virtual void uifopenReqData(Widget, XtPointer);
    virtual void uifrestartComms(Widget, XtPointer);
    virtual void uifsavelayoutNamed(Widget, XtPointer);
    virtual void uifsetTitanParams(Widget, XtPointer);

    static VkComponent *CreateRpmainformClass( const char *name, Widget parent ); 

    //--- End generated code section

    void openLoadNamedLayout();
    void openSaveNamedLayout();


  protected:


    // Classes created by this class

    class RpmainSeqFormClass *_rpmainSeqFormClassUI;
    class RpTabbedDeckClass *_rpTabbedDeckClass;

    // Widgets created by this class

    Widget  _rpmainformClassUI;


    VkWindow * _parent;


    //--- End generated code section



  private:

    // Array of default resources
    static void SaveDispLayoutokCallback(
			Widget w,
			XtPointer clientData, 
			XtPointer callData);
    static void SaveDispLayoutcancelCallback(
			Widget w,
			XtPointer clientData, 
			XtPointer callData);
    static void LoadDispLayoutokCallback(
			Widget w,
			XtPointer clientData, 
			XtPointer callData);
    static void LoadDispLayoutcancelCallback(
			Widget w,
			XtPointer clientData, 
			XtPointer callData);

    void SaveDispLayoutokHandler(
			Widget w,
			XtPointer callData);
    void SaveDispLayoutcancelHandler(
			Widget w,
			XtPointer callData);
    void LoadDispLayoutokHandler(
			Widget w,
			XtPointer callData);
    void LoadDispLayoutcancelHandler(
			Widget w,
			XtPointer callData);

    static void SaveSchedokCallback(     
			Widget w,
			XtPointer clientData, 
			XtPointer callData);

    static void ExitRequestokCallback(     
			Widget w,
			XtPointer clientData, 
			XtPointer callData);

    static void LoadRealtimeokCallback(
			Widget w,
			XtPointer clientData, 
			XtPointer callData);
    static void DelImgokCallback(     
			Widget w,
			XtPointer clientData, 
			XtPointer callData);
    static void DelSeqokCallback(     
			Widget w,
			XtPointer clientData, 
			XtPointer callData);

    static void RestartCommsokCallback(     
			Widget w,
			XtPointer clientData, 
			XtPointer callData);

    static String      _defaultRpmainformClassResources[];

    //--- End generated code section

};
//---- End of generated code

#endif

