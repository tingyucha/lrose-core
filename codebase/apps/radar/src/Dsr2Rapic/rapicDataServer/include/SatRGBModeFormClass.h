
//////////////////////////////////////////////////////////////
//
// Header file for SatRGBModeFormClass
//
//    This file is generated by RapidApp 1.2
//
//    This class implements the user interface portion of a class
//
//    Restrict changes to those sections between
//    the "//--- Start/End editable code block" markers
//
//    This will allow RapidApp to integrate changes more easily
//
//    This class is a ViewKit user interface "component".
//    For more information on how components are used, see the
//    "ViewKit Programmers' Manual", and the RapidApp
//    User's Guide.
//
//////////////////////////////////////////////////////////////
#ifndef SATRGBMODEFORMCLASS_H
#define SATRGBMODEFORMCLASS_H
#include <Vk/VkComponent.h>


//---- Start editable code block: headers and declarations


//---- End editable code block: headers and declarations


class VkOptionMenu;
class VkMenuAction;
class VkMenuToggle;
class VkMenuItem;
class SatRGBModeFormClass : public VkComponent
{ 


  public:

    SatRGBModeFormClass ( const char *, Widget );
    SatRGBModeFormClass ( const char * );
    ~SatRGBModeFormClass();
    void create ( Widget );
    const char *  className();

    static VkComponent *CreateSatRGBModeFormClass( const char *name, Widget parent ); 

    //---- Start editable code block: SatRGBModeFormClass public


    //---- End editable code block: SatRGBModeFormClass public


  protected:


    // Widgets created by this class

    Widget  _button85;
    Widget  _button86;
    Widget  _label65;
    Widget  _label66;
    Widget  _label67;
    Widget  _label68;
    Widget  _satRGBModeFormClass1;

    VkOptionMenu  *_blueChOptionMenu;
    VkOptionMenu  *_greenChOptionMenu;
    VkOptionMenu  *_redChOptionMenu;

    VkMenuItem *_button79;
    VkMenuItem *_button80;
    VkMenuItem *_button81;
    VkMenuItem *_button82;
    VkMenuItem *_button83;
    VkMenuItem *_button84;
    VkMenuItem *_optionA10;
    VkMenuItem *_optionA11;
    VkMenuItem *_optionA12;
    VkMenuItem *_optionB10;
    VkMenuItem *_optionB11;
    VkMenuItem *_optionB12;


    // These virtual functions are called from the private callbacks (below)
    // Intended to be overriden in derived classes to define actions

    virtual void apply ( Widget, XtPointer );
    virtual void blueChChanged ( Widget, XtPointer );
    virtual void close ( Widget, XtPointer );
    virtual void greenChChanged ( Widget, XtPointer );
    virtual void redChChanged ( Widget, XtPointer );

    //---- Start editable code block: SatRGBModeFormClass protected


    //---- End editable code block: SatRGBModeFormClass protected


  private: 

    // Array of default resources

    static String      _defaultSatRGBModeFormClassResources[];


    // Callbacks to interface with Motif

    static void applyCallback ( Widget, XtPointer, XtPointer );
    static void blueChChangedCallback ( Widget, XtPointer, XtPointer );
    static void closeCallback ( Widget, XtPointer, XtPointer );
    static void greenChChangedCallback ( Widget, XtPointer, XtPointer );
    static void redChChangedCallback ( Widget, XtPointer, XtPointer );

    static void* RegisterSatRGBModeFormClassInterface();

    //---- Start editable code block: SatRGBModeFormClass private


    //---- End editable code block: SatRGBModeFormClass private
};
//---- Start editable code block: End of generated code


//---- End editable code block: End of generated code

#endif

