
//////////////////////////////////////////////////////////////
//
// Header file for RpDeckClass
//
//    This file is generated by RapidApp 1.2
//
//    This class is derived from VkTabbedDeck
//
//    When you modify this header file, limit your changes to those
//    areas between the "//---- Start/End editable code block" markers
//
//    This will allow the builder to integrate changes more easily
//
//    This class is a ViewKit user interface "component".
//    For more information on how components are used, see the
//    "ViewKit Programmers' Manual", and the RapidApp
//    User's Guide.
//////////////////////////////////////////////////////////////
#ifndef RPDECKCLASS_H
#define RPDECKCLASS_H
#include <Vk/VkTabbedDeck.h>
//---- Start editable code block: headers and declarations


//---- End editable code block: headers and declarations



//---- RpDeckClass class declaration

class RpDeckClass : public VkTabbedDeck
{

  public:

    RpDeckClass ( const char *, Widget );
    RpDeckClass ( const char * );
    ~RpDeckClass();
    const char *  className();

    //---- Start editable code block: RpDeckClass public


    //---- End editable code block: RpDeckClass public



    protected:


    // Classes created by this class

    class RpCursorDataFormClass *_rpCursorDataFormClassUI;
    class RpSeqSpdDepthFormClass *_rpSeqSpdDepthFormClassUI;

    // Widgets created by this class

    Widget  _rpDeckClass;


    //---- Start editable code block: RpDeckClass protected


    //---- End editable code block: RpDeckClass protected



  private: 

    // Array of default resources

    static String      _defaultRpDeckClassResources[];

    //---- Start editable code block: RpDeckClass private


    //---- End editable code block: RpDeckClass private
};
//---- Start editable code block: End of generated code


//---- End editable code block: End of generated code

#endif

