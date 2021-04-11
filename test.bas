
open "Release\LBTBColor.dll" for DLL as #lbtbc

'Excel Good/Bad/Neutral cell colors
goodTextClr    = RGB(  0,  97,   0)
goodBGClr      = RGB(198, 239, 206)

badTextClr     = RGB(156,   0,   6)
badBGClr       = RGB(255, 199, 206)

neutralTextClr = RGB(156,  87,   0)
neutralBGClr   = RGB(255, 235, 156)

nomainwin

[setup.m.Window]
    '-----Begin code for #m

    WindowWidth = 450
    WindowHeight = 415
    UpperLeftX=int((DisplayWidth-WindowWidth)/2)
    UpperLeftY=int((DisplayHeight-WindowHeight)/2)


    '-----Begin GUI objects code

    groupbox #m.groupbox6, "Custom Colors",  10, 142, 415, 220
    TextboxColor$ = "white"
    textbox #m.tbGood,  10,  12, 100,  25
    textbox #m.tbNeutral, 160,  12, 100,  25
    textbox #m.tbBad, 315,  12, 100,  25
    textbox #m.tbCustom,  10, 107, 415,  25
    statictext #m.statictext8, "Text Red:",  20, 172,  58,  20
    textbox #m.tbCusTextRed, 100, 167, 100,  25
    statictext #m.statictext10, "Text Green:",  20, 207,  69,  20
    statictext #m.statictext11, "Text Blue:",  20, 242,  59,  20
    textbox #m.tbCusTextGreen, 100, 202, 100,  25
    textbox #m.tbCusTextBlue, 100, 237, 100,  25
    statictext #m.statictext14, "BG Red:", 215, 172,  51,  20
    statictext #m.statictext15, "BG Green:", 215, 207,  62,  20
    statictext #m.statictext16, "BG Blue:", 215, 242,  52,  20
    textbox #m.tbCusBGRed, 285, 167, 100,  25
    textbox #m.tbCusBGGreen, 285, 202, 100,  25
    textbox #m.tbCusBGBlue, 285, 237, 100,  25
    button #m.button20,"Set Custom Textbox Colors",[setColors], UL,  30, 287, 180,  25
    button #m.button21,"Reset Custom Textbox",[resetColors], UL, 230, 287, 170,  25
    button #m.button22,"Set Custom Text Color Only",[setTextColor], UL,  30, 322, 179,  25
    button #m.button23,"Set Custom BG Color Only",[setBGColor], UL, 230, 322, 172,  25

    '-----End GUI objects code

    open "Test Textbox Colors" for window as #m
    print #m, "font ms_sans_serif 10"
    print #m, "trapclose [quit.m]"

    #m.tbCusTextRed,   "0"
    #m.tbCusTextGreen, "0"
    #m.tbCusTextBlue,  "0"
    #m.tbCusBGRed,     "255"
    #m.tbCusBGGreen,   "255"
    #m.tbCusBGBlue,    "255"

    #m.tbGood,    "Good"
    #m.tbNeutral, "Neutral"
    #m.tbBad,     "Bad"
    #m.tbCustom,  "Custom"

    hTBGood =    hwnd(#m.tbGood)
    hTBNeutral = hwnd(#m.tbNeutral)
    hTBBad =     hwnd(#m.tbBad)
    hTBCustom =  hwnd(#m.tbCustom)

    a = SetTextboxColors(hTBGood, goodTextClr, goodBGClr)
    a = SetTextboxColors(hTBNeutral, neutralTextClr, neutralBGClr)
    a = SetTextboxTextColor(hTBBad, badTextClr)
    a = SetTextboxBGColor(hTBBad, badBGClr)

[m.inputLoop]   'wait here for input event
    wait

[setColors]
    #m.tbCusTextRed, "!contents? cusTextRed"
    #m.tbCusTextGreen, "!contents? cusTextGreen"
    #m.tbCusTextBlue, "!contents? cusTextBlue"
    #m.tbCusBGRed, "!contents? cusBGRed"
    #m.tbCusBGGreen, "!contents? cusBGGreen"
    #m.tbCusBGBlue, "!contents? cusBGBlue"

    a = SetTextboxColorsRGB(hTBCustom, cusTextRed, cusTextGreen, cusTextBlue,_
        cusBGRed, cusBGGreen, cusBGBlue)
    wait

[resetColors]
    a = ResetTextboxColors(hTBCustom)
    wait

[setTextColor]
    #m.tbCusTextRed, "!contents? cusTextRed"
    #m.tbCusTextGreen, "!contents? cusTextGreen"
    #m.tbCusTextBlue, "!contents? cusTextBlue"

    a = SetTextboxTextColorRGB(hTBCustom, cusTextRed, cusTextGreen, cusTextBlue)
    wait

[setBGColor]
    #m.tbCusBGRed, "!contents? cusBGRed"
    #m.tbCusBGGreen, "!contents? cusBGGreen"
    #m.tbCusBGBlue, "!contents? cusBGBlue"

    a = SetTextboxBGColorRGB(hTBCustom, cusBGRed, cusBGGreen, cusBGBlue)
    wait


[quit.m] 'End the program
    close #m
    close #lbtbc
    end

Function RGB(r, g, b)
    RGB = ((b AND 255) * 65536) + ((g AND 255) * 256) + (r AND 255)
End Function

Function SetTextboxColors(hTB, textColor, bgColor)
    CallDLL #lbtbc, "SetTextboxColors",_
    hTB as ulong, textColor as long,_
    bgColor as long, SetTextboxColors as long
End Function

Function SetTextboxColorsRGB(hTB, textR, textG, textB, bgR, bgG, bgB)
    CallDLL #lbtbc, "SetTextboxColorsRGB",_
    hTB as ulong, textR as long, textG as long,_
    textB as long, bgR as long, bgG as long,_
    bgB as long, SetTextboxColorsRGB as long
End Function

Function SetTextboxTextColor(hTB, textColor)
    CallDLL #lbtbc, "SetTextboxTextColor",_
    hTB as ulong, textColor as long,_
    SetTextboxTextColor as long
End Function

Function SetTextboxTextColorRGB(hTB, textR, textG, textB)
    CallDLL #lbtbc, "SetTextboxTextColorRGB",_
    hTB as ulong, textR as long, textG as long,_
    textB as long, SetTextboxTextColorRGB as long
End Function

Function SetTextboxBGColor(hTB, bgColor)
    CallDLL #lbtbc, "SetTextboxBGColor",_
    hTB as ulong, bgColor as long,_
    SetTextboxBGColor as long
End Function

Function SetTextboxBGColorRGB(hTB, bgR, bgG, bgB)
    CallDLL #lbtbc, "SetTextboxBGColorRGB",_
    hTB as ulong, bgR as long, bgG as long,_
    bgB as long, SetTextboxBGColorRGB as long
End Function

Function ResetTextboxColors(hTB)
    CallDLL #lbtbc, "ResetTextboxColors",_
    hTB as ulong, ResetTextboxColors as long
End Function
