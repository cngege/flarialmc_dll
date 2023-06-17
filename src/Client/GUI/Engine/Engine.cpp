﻿#include "Engine.hpp"
#include "Constraints.hpp"
#include "../../Module/Modules/Module.hpp"
#include "../../Hook/Hooks/Render/SwapchainHook.hpp"
#include "animations/fadeinout.hpp"

std::map<std::string, ID2D1Bitmap*> ImagesClass::eimages;

float maxDarkenAmount = 0.1;

static bool CursorInRect(float rectX, float rectY, float width, float height)
{
    if (MC::mousepos.x >= rectX && MC::mousepos.x <= rectX + width && MC::mousepos.y >= rectY && MC::mousepos.y <= rectY + height)
    {
        return true;
    }
    return false;
}

static bool CursorInEllipse(float ellipseX, float ellipseY, float radiusX, float radiusY)
{
    float mouseX = MC::mousepos.x;
    float mouseY = MC::mousepos.y;

    float normalizedX = (mouseX - ellipseX) / radiusX;
    float normalizedY = (mouseY - ellipseY) / radiusY;

    return (normalizedX * normalizedX + normalizedY * normalizedY) <= 1.0f;
}


void FlarialGUI::PushSize(float x, float y, float width, float height)
{
    Dimension size;
    size.x = x;
    size.y = y;
    size.width = width;
    size.height = height;

    dimension_stack.push(size);
}

void FlarialGUI::PopSize()
{
    dimension_stack.pop();
}

void FlarialGUI::PopAllStack()
{
    if(!dimension_stack.empty())
    {
        dimension_stack.pop();
    }
}

bool FlarialGUI::Button(float x, float y, const D2D_COLOR_F color, const D2D_COLOR_F textColor, const wchar_t *text, const float width, const float height)
{
    if (isInScrollView)
        y += scrollpos;



    ID2D1SolidColorBrush *brush;
    D2D1_COLOR_F buttonColor = CursorInRect(x, y, width, height) ? D2D1::ColorF(color.r - darkenAmounts[x+y], color.g - darkenAmounts[x+y], color.b - darkenAmounts[x+y], color.a) : color;
    D2D::context->CreateSolidColorBrush(buttonColor, &brush);
    D2D_RECT_F rect = D2D1::RectF(x, y, x + width, y + height);


    D2D::context->FillRectangle(rect, brush);
    brush->Release();



    IDWriteTextFormat *textFormat;
    IDWriteFactory *writeFactory;
    DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown **>(&writeFactory));
    writeFactory->CreateTextFormat(L"Arial", NULL, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 14.0f, L"en-US", &textFormat); ID2D1SolidColorBrush *textBrush; D2D::context->CreateSolidColorBrush(textColor, &textBrush); textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER); textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER); D2D::context->DrawText(text, (UINT32)wcslen(text), textFormat, D2D1::RectF(x, y, x + width, y + height), textBrush); textBrush->Release(); textFormat->Release();

    if (CursorInRect(x, y, width, height) && MC::mousebutton == MouseButton::Left && !MC::held)
    {
        MC::mousebutton = MouseButton::None;
        return true;
    }
    else
    {
        MC::mousebutton = MouseButton::None;
        return false;
    }
}


            
bool FlarialGUI::RoundedButton(float x, float y, const D2D_COLOR_F color, const D2D_COLOR_F textColor, const wchar_t *text, const float width, const float height, float radiusX, float radiusY)
{
    if (isInScrollView)
        y += scrollpos;

    static IDWriteFactory* writeFactory;
    DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown **>(&writeFactory));

    static ID2D1SolidColorBrush* textBrush;
    D2D::context->CreateSolidColorBrush(textColor, &textBrush);

    static IDWriteTextFormat* textFormat;
    writeFactory->CreateTextFormat(L"Space Grotesk", NULL, DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, Constraints::FontScaler(width, height), L"en-US", &textFormat);
    textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
    textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

    ID2D1SolidColorBrush* brush = nullptr;
    D2D1_COLOR_F buttonColor;

    if (CursorInRect(x, y, width, height))
    {
        buttonColor = D2D1::ColorF(color.r - darkenAmounts[x + y], color.g - darkenAmounts[x + y], color.b - darkenAmounts[x + y], color.a);
        FadeEffect::ApplyFadeInEffect(0.005 * FlarialGUI::frameFactor, maxDarkenAmount, darkenAmounts[x+y]);
    } else {
        buttonColor = D2D1::ColorF(color.r - darkenAmounts[x + y], color.g - darkenAmounts[x + y], color.b - darkenAmounts[x + y], color.a);
        FadeEffect::ApplyFadeOutEffect(0.005 * FlarialGUI::frameFactor, darkenAmounts[x + y]);

    }

    D2D::context->CreateSolidColorBrush(buttonColor, &brush);

    D2D1_ROUNDED_RECT roundedRect = D2D1::RoundedRect(D2D1::RectF(x, y, x + width, y + height), radiusX, radiusY);
    D2D::context->FillRoundedRectangle(roundedRect, brush);

    D2D::context->DrawText(text, (UINT32)wcslen(text), textFormat, D2D1::RectF(x, y, x + width, y + height), textBrush);

    brush->Release();
    writeFactory->Release();
    textFormat->Release();
    textBrush->Release();

    if (CursorInRect(x, y, width, height) && MC::mousebutton == MouseButton::Left && !MC::held)
    {
        MC::mousebutton = MouseButton::None;
        return true;

    }

    return false;
}

bool FlarialGUI::RoundedRadioButton(float x, float y, const D2D_COLOR_F color, const D2D_COLOR_F textColor, const wchar_t *text, const float width, const float height, float radiusX, float radiusY, const std::string& radioNum, const std::string& currentNum)
{
    if (isInScrollView)
        y += scrollpos;

    static IDWriteFactory* writeFactory;
    DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown **>(&writeFactory));

    static ID2D1SolidColorBrush* textBrush;
    D2D::context->CreateSolidColorBrush(textColor, &textBrush);

    static IDWriteTextFormat* textFormat;
    writeFactory->CreateTextFormat(L"Space Grotesk", NULL, DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, Constraints::FontScaler(width * 0.64, height * 0.64), L"en-US", &textFormat);
    textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
    textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

    ID2D1SolidColorBrush* brush = nullptr;
    D2D1_COLOR_F buttonColor = color;


    if(radioNum != currentNum) {
        FadeEffect::ApplyFadeInEffect(0.03 * FlarialGUI::frameFactor, 1, opacityAmounts[x / y]);
        buttonColor = D2D1::ColorF(color.r, color.g, color.b, color.a - opacityAmounts[x / y]);
    }
    else {
        FadeEffect::ApplyFadeOutEffect(0.03 * FlarialGUI::frameFactor, opacityAmounts[x / y]);
        buttonColor = D2D1::ColorF(color.r, color.g, color.b, color.a - opacityAmounts[x / y]);
    }


    D2D::context->CreateSolidColorBrush(buttonColor, &brush);

    D2D1_ROUNDED_RECT roundedRect = D2D1::RoundedRect(D2D1::RectF(x, y, x + width, y + height), radiusX, radiusY);
    D2D::context->FillRoundedRectangle(roundedRect, brush);

    D2D::context->DrawText(text, (UINT32)wcslen(text), textFormat, D2D1::RectF(x, y, x + width, y + height), textBrush);

    brush->Release();
    writeFactory->Release();
    textFormat->Release();
    textBrush->Release();

    if (CursorInRect(x, y, width, height) && MC::mousebutton == MouseButton::Left && !MC::held)
    {
        MC::mousebutton = MouseButton::None;
        return true;
    }

    return false;
}

void FlarialGUI::RoundedRect(float x, float y, const D2D_COLOR_F color, const float width, const float height, float radiusX, float radiusY)
{
    if (isInScrollView)
        y += scrollpos;

    ID2D1SolidColorBrush *brush;
    D2D::context->CreateSolidColorBrush(color, &brush);
    D2D1_ROUNDED_RECT roundedRect = D2D1::RoundedRect(D2D1::RectF(x, y, x + width, y + height), radiusX, radiusY);

    D2D::context->FillRoundedRectangle(roundedRect, brush);
    brush->Release();
}

void FlarialGUI::RoundedRectOnlyTopCorner(float x, float y, D2D_COLOR_F color, float width, float height, float radiusX, float radiusY)
{
    if (isInScrollView)
        y += scrollpos;

    D2D_RECT_F rect = D2D1::RectF(x, y, x + width, y + height);

    ID2D1Factory *factory;
    D2D::context->GetFactory(&factory);

    ID2D1PathGeometry* geometry = nullptr;
    factory->CreatePathGeometry(&geometry);

    ID2D1GeometrySink* sink = nullptr;
    geometry->Open(&sink);

    D2D1_POINT_2F startPoint = D2D1::Point2F(rect.left + radiusX, rect.top);
    D2D1_POINT_2F topRightArcEndPoint = D2D1::Point2F(rect.right - radiusX, rect.top);
    D2D1_POINT_2F endPoint = D2D1::Point2F(rect.right, rect.bottom);

    sink->BeginFigure(startPoint, D2D1_FIGURE_BEGIN_FILLED);

    // Top line (first segment)
    sink->AddLine(topRightArcEndPoint);

    // Top right arc
    D2D1_ARC_SEGMENT arc2;
    arc2.point = D2D1::Point2F(rect.right, rect.top + radiusY);
    arc2.size = D2D1::SizeF(radiusX, radiusY);
    arc2.rotationAngle = 90.0f;
    arc2.sweepDirection = D2D1_SWEEP_DIRECTION_CLOCKWISE;
    arc2.arcSize = D2D1_ARC_SIZE_SMALL;
    sink->AddArc(arc2);

    // Top line (second segment)
    sink->AddLine(endPoint);

    // Right line
    sink->AddLine(D2D1::Point2F(rect.right, rect.bottom));

    // Bottom line
    sink->AddLine(D2D1::Point2F(rect.left, rect.bottom));

    // Left line
    sink->AddLine(D2D1::Point2F(rect.left, rect.top + radiusY));

    // Bottom left arc
    D2D1_ARC_SEGMENT arc3;
    arc3.point = startPoint;
    arc3.size = D2D1::SizeF(radiusX, radiusY);
    arc3.rotationAngle = 90.0f;
    arc3.sweepDirection = D2D1_SWEEP_DIRECTION_CLOCKWISE;
    arc3.arcSize = D2D1_ARC_SIZE_SMALL;
    sink->AddArc(arc3);

    sink->EndFigure(D2D1_FIGURE_END_CLOSED);
    sink->Close();

    ID2D1SolidColorBrush* brush;
    D2D::context->CreateSolidColorBrush(color, &brush);

    D2D::context->FillGeometry(geometry, brush);

    Memory::SafeRelease(brush);
    Memory::SafeRelease(sink);
    Memory::SafeRelease(geometry);
    Memory::SafeRelease(factory);
}


bool FlarialGUI::Toggle(float x, float y, const D2D1_COLOR_F color, const D2D1_COLOR_F circleColor, bool isEnabled) {

    float rectWidth = Constraints::RelativeConstraint(0.062);
    float rectHeight = Constraints::RelativeConstraint(0.03);

    Vec2<float> round = Constraints::RoundingConstraint(18, 18);

    FlarialGUI::RoundedRect(x, y, color, rectWidth, rectHeight, round.x, round.x);

    // the circle (I KNOW IM USING A RECT LOL)

    float circleWidth = Constraints::RelativeConstraint(0.0202);
    float circleHeight = Constraints::RelativeConstraint(0.02);


    float ySpacing = Constraints::SpacingConstraint(0.2, circleHeight);
    float xSpacing = Constraints::SpacingConstraint(0.2, circleWidth);
    round = Constraints::RoundingConstraint(23, 23);

    float enabledSpacing;

    if(isEnabled) {
        FadeEffect::ApplyFadeInEffect(2.4 * FlarialGUI::frameFactor, Constraints::SpacingConstraint(1.6, circleWidth), FlarialGUI::toggleSpacings[x+y]);
        enabledSpacing =  FlarialGUI::toggleSpacings[x+y];
    } else {
        FadeEffect::ApplyFadeOutEffect(2.4 * FlarialGUI::frameFactor, FlarialGUI::toggleSpacings[x+y]);
        enabledSpacing =  FlarialGUI::toggleSpacings[x+y];
    }

    FlarialGUI::RoundedRect(x + xSpacing + enabledSpacing, y + ySpacing, circleColor, circleWidth, circleHeight, round.x, round.x);

    if (CursorInRect(x, y, rectWidth, rectHeight) && MC::mousebutton == MouseButton::Left && !MC::held)
    {
        MC::mousebutton = MouseButton::None;
        return true;
    }

    return false;
}

float FlarialGUI::Slider(int index, float x, float y, const D2D1_COLOR_F color, const D2D1_COLOR_F disabledColor, const D2D1_COLOR_F circleColor) {

    // Define the total slider rect width and height
    const float totalWidth = Constraints::RelativeConstraint(0.2);
    const float height = Constraints::RelativeConstraint(0.015);

    // Calculate the farLeftX and farRightX
    const float farLeftX = x;
    float farRightX = x + totalWidth;

    // Calculate the position of the circle in the middle of the slider rect
    const float circleRadius = Constraints::RelativeConstraint(0.015);

    float circleX = x + totalWidth / 2.0f;
    float circleY = y + height / 2.0f;

    float rectangleLeft = farLeftX;
    float rectangleWidth = farRightX - farLeftX;

    float maxValue = 100.0f; // Maximum value (e.g., 100%)
    float minValue = 0.0f;   // Minimum value (e.g., 0%)


    if(SliderRects[index].hasBeenMoved) {


        circleX = (SliderRects[index].percentageX - minValue) * (rectangleWidth / (maxValue - minValue)) + rectangleLeft;

    }

    // Calculate the position and width of the enabled portion rect
    const float enabledWidth = circleX - farLeftX;

    Vec2<float> round = Constraints::RoundingConstraint(9, 9);

    // Draw the disabled portion rect
    RoundedRect(farLeftX, y, disabledColor, totalWidth, height, round.x,round.x);

    // Draw the enabled portion rect
    RoundedRect(farLeftX, y, color, enabledWidth, height, round.x, round.x);
    // Draw the circle in the middle

    FlarialGUI::Circle(circleX, circleY, circleColor, circleRadius);

    // Calculate the percentage
    float percentage = ((circleX - rectangleLeft) / rectangleWidth) * (maxValue - minValue) + minValue;

    if(percentage > 0) percentage = 0;

    if (CursorInEllipse(circleX, circleY, circleRadius, circleRadius) && MC::held) {
        if(MC::mousepos.x > farLeftX && MC::mousepos.x < farRightX) {

            SliderRects[index].movedX = MC::mousepos.x - circleRadius / 2.0f;
            SliderRects[index].hasBeenMoved = true;
            SliderRects[index].isMovingElement = true;
        }

        if(MC::mousepos.x < farLeftX) SliderRects[index].movedX = farLeftX;
        else if(MC::mousepos.x > farRightX) SliderRects[index].movedX = farRightX;

        percentage = ((SliderRects[index].movedX - rectangleLeft) / rectangleWidth) * (maxValue - minValue) + minValue;
        SliderRects[index].percentageX = percentage;

    } else if (MC::held && SliderRects[index].isMovingElement) {

        if(MC::mousepos.x > farLeftX && MC::mousepos.x < farRightX) {

            SliderRects[index].movedX = MC::mousepos.x - circleRadius / 2.0f;
            SliderRects[index].hasBeenMoved = true;
            SliderRects[index].isMovingElement = true;
        }

        if(MC::mousepos.x < farLeftX) SliderRects[index].movedX = farLeftX;
        else if(MC::mousepos.x > farRightX) SliderRects[index].movedX = farRightX;

        percentage = ((SliderRects[index].movedX - rectangleLeft) / rectangleWidth) * (maxValue - minValue) + minValue;
        SliderRects[index].percentageX = percentage;
    }

    if (MC::mousebutton == MouseButton::None && !MC::held || MC::mousebutton == MouseButton::Left && !MC::held)
    {
        SliderRects[index].isMovingElement = false;
    }

    return percentage;

}

void FlarialGUI::Circle(float x, float y, const D2D1_COLOR_F& color, float radius) {
    // Assuming D2D::context is the ID2D1DeviceContext object

    // Create a brush using the specified color
    ID2D1SolidColorBrush* brush;
    D2D::context->CreateSolidColorBrush(color, &brush);

    // Create an ellipse with the specified parameters
    D2D1_ELLIPSE ellipse;
    ellipse.point = D2D1::Point2F(x, y);
    ellipse.radiusX = radius;
    ellipse.radiusY = radius;

    // Draw the ellipse using the device context and brush
    D2D::context->FillEllipse(ellipse, brush);

    // Release the brush
    brush->Release();

    // Rest of your code...
}





void FlarialGUI::RoundedRectWithImageAndText(float x, float y, const float width, const float height, const D2D1_COLOR_F color, const std::string imagePath, const float imageWidth, const float imageHeight, const wchar_t *text)
{

    float imageY = y;

    if (isInScrollView) {
        y += scrollpos;
        imageY += scrollpos;
    }


    ID2D1SolidColorBrush *brush;
    D2D::context->CreateSolidColorBrush(color, &brush);

    D2D1_ROUNDED_RECT roundedRect = D2D1::RoundedRect(D2D1::RectF(x, y, x + width, y + height), 5, 5);

    D2D::context->FillRoundedRectangle(roundedRect, brush);

    x = x + (width - imageWidth) / 2.0f;
    imageY = imageY + (height - imageHeight) / 2.0f;

    D2D1_RECT_F  imagerect = D2D1::RectF(x, imageY, x + imageWidth, imageY + imageHeight);

    if (ImagesClass::eimages[imagePath] == nullptr) {

        std::string among = Utils::getRoamingPath() + "\\" + imagePath;
        FlarialGUI::LoadImageFromFile(to_wide(among).c_str(), &ImagesClass::eimages[imagePath]);

    } else if (ImagesClass::eimages[imagePath] != nullptr) {
        D2D::context->DrawBitmap(ImagesClass::eimages[imagePath], imagerect);
    }

    /*
    // Draw text
    IDWriteFactory *writeFactory;
    DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown **>(&writeFactory));
    IDWriteTextFormat *textFormat;
    writeFactory->CreateTextFormat(L"Space Grotesk", NULL, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 14.0f, L"", &textFormat);
    textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
    textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

    D2D1_RECT_F textRect = D2D1::RectF(x + height + 10, y, x + width, y + height);
    D2D::context->DrawText(text, (UINT32)wcslen(text), textFormat, textRect, brush);*/

    brush->Release();/*
    textFormat->Release();
    writeFactory->Release();*/
}


void FlarialGUI::FlarialText(float x, float y, const wchar_t *text, D2D1_COLOR_F color, const float width, const float height)
{

    if (isInScrollView)
        y += scrollpos;
    ID2D1SolidColorBrush *brush;
    D2D::context->CreateSolidColorBrush(color, &brush);

    IDWriteFactory *writeFactory;
    DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown **>(&writeFactory));
    IDWriteTextFormat *textFormat;
    writeFactory->CreateTextFormat(L"Space Grotesk", NULL, DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, Constraints::FontScaler(width, height), L"", &textFormat);
    textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
    textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

    D2D1_RECT_F textRect = D2D1::RectF(x + height + 10, y, x + width, y + height);
    D2D::context->DrawText(text, (UINT32)wcslen(text), textFormat, textRect, brush);

    writeFactory->Release();
    textFormat->Release();
    brush->Release();
}

void FlarialGUI::Image(const std::string imageName, D2D1_RECT_F rect)
{
    if (isInScrollView) {
        rect.top += scrollpos;
        rect.bottom += scrollpos;
    }
    std::string among = Utils::getRoamingPath() + "\\" + imageName;

    if(ImagesClass::eimages[imageName] == nullptr)
    LoadImageFromFile(to_wide(among).c_str(), &ImagesClass::eimages[imageName]);

    // Draw image
    D2D1_RECT_F imageRect = D2D1::RectF(rect.left, rect.top, rect.right, rect.bottom);
    D2D::context->DrawBitmap(ImagesClass::eimages[imageName], imageRect, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
}

void FlarialGUI::LoadImageFromFile(const wchar_t *filename, ID2D1Bitmap **bitmap)
{
    // Initialize COM
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

    IWICBitmapDecoder *bitmapDecoder = nullptr;
    IWICBitmapFrameDecode *frame = nullptr;
    IWICFormatConverter *formatConverter = nullptr;

    IWICImagingFactory *imagingFactory = nullptr;
    CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&imagingFactory));

    // Create decoder
    imagingFactory->CreateDecoderFromFilename(filename, nullptr, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &bitmapDecoder);

    // Get first frame
    bitmapDecoder->GetFrame(0, &frame);

    // Convert format to 32bppPBGRA
    imagingFactory->CreateFormatConverter(&formatConverter);
    formatConverter->Initialize(frame, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, nullptr, 0.0, WICBitmapPaletteTypeMedianCut);

    // Create bitmap
    D2D::context->CreateBitmapFromWicBitmap(formatConverter, nullptr, bitmap);

    bitmapDecoder->Release();
    frame->Release();
    formatConverter->Release();
    imagingFactory->Release();
}

void FlarialGUI::SetScrollView(float x, float y, float width, float height)
{
    FlarialGUI::isInScrollView = true;
    D2D1_RECT_F clipRect = D2D1::RectF(x, y, x + width, y + height);
    D2D::context->PushAxisAlignedClip(&clipRect, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
}

void FlarialGUI::UnsetScrollView()
{
    FlarialGUI::isInScrollView = false;
    D2D::context->PopAxisAlignedClip();
}

void FlarialGUI::ScrollBar(float x, float y, float width, float height, float radius)
{

    float whiteY;

    if (y - barscrollpos < y + (height * 30.5 / 100))
        whiteY = y - (barscrollpos);
    else
    {
        whiteY = y + (height * 30.5 / 100);
        barscrollpos += barscrollposmodifier;
        scrollpos += scrollposmodifier;
    }

    if (y + barscrollpos > y)
    {
        whiteY = y;
        barscrollpos = 0;
        scrollpos = 0;
    }

    // Draw the gray bar
    ID2D1SolidColorBrush *graybrush;
    D2D::context->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Gray, 0.0), &graybrush);
    D2D1_ROUNDED_RECT grayRect = D2D1::RoundedRect(D2D1::RectF(x, y, x + width, y + height), radius, radius);
    D2D::context->FillRoundedRectangle(&grayRect, graybrush);
    graybrush->Release();
    // Draw the white bar
    ID2D1SolidColorBrush *whitebrush;
    D2D::context->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White, 0.0), &whitebrush);
    D2D1_ROUNDED_RECT whiteRect = D2D1::RoundedRect(D2D1::RectF(x, whiteY, x + width, whiteY + (height * 69.5 / 100)), radius, radius);
    D2D::context->FillRoundedRectangle(&whiteRect, whitebrush);
    whitebrush->Release();
}

void FlarialGUI::SetWindowRect(float x, float y, float width, float height, int currentNum)
{
    isInWindowRect = true;

    if (WindowRects[currentNum].hasBeenMoved)
    {
        x = Constraints::PercentageConstraint(WindowRects[currentNum].percentageX, "left");
        y = Constraints::PercentageConstraint(WindowRects[currentNum].percentageY, "top");
    }

    if (CursorInRect(x, y, width, height) && MC::held)
    {
        WindowRects[currentNum].isMovingElement = true;
        WindowRects[currentNum].hasBeenMoved = true;
        WindowRects[currentNum].movedX = MC::mousepos.x - width / 2.0f;
        WindowRects[currentNum].movedY = MC::mousepos.y - height / 2.0f;
        WindowRects[currentNum].percentageX = WindowRects[currentNum].movedX / MC::windowSize.x;
        WindowRects[currentNum].percentageY = WindowRects[currentNum].movedY / MC::windowSize.y;
    }
    else if (MC::held && WindowRects[currentNum].isMovingElement)
    {
        WindowRects[currentNum].isMovingElement = true;
        WindowRects[currentNum].hasBeenMoved = true;
        WindowRects[currentNum].movedX = MC::mousepos.x - width / 2.0f;
        WindowRects[currentNum].movedY = MC::mousepos.y - height / 2.0f;

        WindowRects[currentNum].percentageX = WindowRects[currentNum].movedX / MC::windowSize.x;
        WindowRects[currentNum].percentageY = WindowRects[currentNum].movedY / MC::windowSize.y;
    }

    if (MC::mousebutton == MouseButton::None && !MC::held || MC::mousebutton == MouseButton::Left && !MC::held)
    {
        WindowRects[currentNum].isMovingElement = false;
    }
}

void FlarialGUI::UnsetWindowRect()
{
    isInWindowRect = false;
}

void FlarialGUI::UpdateWindowRects()
{
    for (int i = 0; i < static_cast<int>(sizeof WindowRects); i++)
    {
        WindowRect &rect = WindowRects[i];
        if (rect.hasBeenMoved)
        {
            rect.movedX = Constraints::PercentageConstraint(rect.percentageX, "left");
            rect.movedY = Constraints::PercentageConstraint(rect.percentageY, "top");
        }
        else
        {
            return;
        }
    }
}

Vec2<float> FlarialGUI::CalculateMovedXY(float x, float y, int num)
{
    if (isInWindowRect && WindowRects[num].hasBeenMoved)
    {
        x = Constraints::PercentageConstraint(WindowRects[num].percentageX, "left");
        y = Constraints::PercentageConstraint(WindowRects[num].percentageY, "top");
    }

    return {x, y};
}

Vec2<float> FlarialGUI::GetCenterXY(float rectWidth, float rectHeight)
{
    Vec2<float> xy;
    xy.x = (D2D::context->GetSize().width - rectWidth) / 2.0f;
    xy.y = (D2D::context->GetSize().height - rectHeight) / 2.0f;
    return xy;
}

Vec2<float> FlarialGUI::CalculateResizedXY(float x, float y, float width, float height)
{
    if (MC::hasResized)
    {
        x = (MC::windowSize.x - width) / 2;
        y = (MC::windowSize.y - height) / 2;
    }

    return Vec2(x, y);
}

void FlarialGUI::ApplyGaussianBlur(float blurIntensity)
{

    // Create Gaussian blur effect
    if(FlarialGUI::blur == nullptr) {

        D2D::context->CreateEffect(CLSID_D2D1GaussianBlur, &FlarialGUI::blur);
    }

    if(SwapchainHook::init) {

        ID2D1Bitmap *bitmap = nullptr;
        if(SwapchainHook::queue != nullptr) FlarialGUI::CopyBitmap(SwapchainHook::D2D1Bitmaps[SwapchainHook::currentBitmap], &bitmap);
        else FlarialGUI::CopyBitmap(SwapchainHook::D2D1Bitmap, &bitmap);

        FlarialGUI::blur->SetInput(0, bitmap);

        // Set blur intensity
        FlarialGUI::blur->SetValue(D2D1_GAUSSIANBLUR_PROP_BORDER_MODE, D2D1_BORDER_MODE_HARD);
        FlarialGUI::blur->SetValue(D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION, blurIntensity);
        // Draw the image with the Gaussian blur effect
        D2D::context->DrawImage(FlarialGUI::blur);

        Memory::SafeRelease(bitmap);
        Memory::SafeRelease(FlarialGUI::blur);
    }
}

void FlarialGUI::CopyBitmap(ID2D1Bitmap1* from, ID2D1Bitmap** to)
{
    if (from == nullptr)
    {
        Logger::debug("from is nullptr");
        return;  // Handle the case where 'from' is nullptr
    }

    if (*to == nullptr)
    {
        D2D1_BITMAP_PROPERTIES props = D2D1::BitmapProperties(from->GetPixelFormat());
        HRESULT hr = D2D::context->CreateBitmap(from->GetPixelSize(), props, to);
        if (FAILED(hr))
        {
            Logger::debug("Failed to create bitmap");
            return;  // Handle the failure to create the bitmap
        }
    }
    else if (from->GetPixelSize() != (*to)->GetPixelSize())
    {
        (*to)->Release();
        D2D1_BITMAP_PROPERTIES props = D2D1::BitmapProperties(from->GetPixelFormat());
        HRESULT hr = D2D::context->CreateBitmap(from->GetPixelSize(), props, to);
        if (FAILED(hr))
        {
            Logger::debug("Failed to create bitmap");
            return;  // Handle the failure to create the bitmap
        }
    }

    D2D1_POINT_2U destPoint = { 0, 0 };
    D2D1_SIZE_U size = from->GetPixelSize();
    D2D1_RECT_U rect = { 0, 0, size.width, size.height };
    (*to)->CopyFromBitmap(&destPoint, from, &rect);
}


std::wstring FlarialGUI::to_wide(const std::string &multi)
{
    std::wstring wide;
    wchar_t w;
    mbstate_t mb{};
    size_t n = 0, len = multi.length() + 1;
    while (auto res = mbrtowc(&w, multi.c_str() + n, len - n, &mb))
    {
        if (res == size_t(-1) || res == size_t(-2))
            throw "invalid encoding";

        n += res;
        wide += w;
    }
    return wide;
}