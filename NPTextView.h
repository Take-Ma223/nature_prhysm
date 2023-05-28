#pragma once
#include "TextView.h"
#include "View.h"
#include "FontSet.h"
#include <memory>
#include "NPTextViewParam.h"

using namespace std;

class NPTextView : public View
{
public:
    NPTextView(ActivityContext* c, NPTextViewParam npTextViewInitParam = NPTextViewParam(), DrawableInitParam drawableInitParam = DrawableInitParam()) : View(c, drawableInitParam) {
        initText(c, npTextViewInitParam, drawableInitParam);
    }
    NPTextView(Drawable* parent, ActivityContext* c, NPTextViewParam npTextViewInitParam = NPTextViewParam(), DrawableInitParam drawableInitParam = DrawableInitParam()) : View(parent, c, drawableInitParam) {
        initText(c, npTextViewInitParam, drawableInitParam);
    }
    ~NPTextView() {
        //DeleteGraph(textImageHandle.getHandle());
    }

    void setText(NPTextViewParam p) {
        frontText.get()->setText(getFrontTextViewParam(p));
        shadowText.get()->setText(getShadowTextViewParam(p));
        makeScreen(Size(frontText.get()->getSize().x + 2, frontText.get()->getSize().y + 2));
    }

protected:
    virtual void initText(ActivityContext* c, NPTextViewParam npTextViewInitParam = NPTextViewParam(), DrawableInitParam drawableInitParam = DrawableInitParam()) {

        DrawableInitParam frontDrawableParam = DrawableInitParam(Cordinate(0, 0));
        frontText = unique_ptr<TextView>(new
            TextView(
                c,
                getFrontTextViewParam(npTextViewInitParam),
                frontDrawableParam
            )
        );

        DrawableInitParam shadowDrawableParam = DrawableInitParam(Cordinate(2, 2));
        shadowText = unique_ptr<TextView>(new
            TextView(
                c,
                getShadowTextViewParam(npTextViewInitParam),
                shadowDrawableParam
            )
        );

        makeScreen(Size(frontText.get()->getSize().x + 2, frontText.get()->getSize().y + 2));

        addDrawable(shadowText.get());
        addDrawable(frontText.get());
    }

private:
    unique_ptr<TextView> frontText;
    unique_ptr<TextView> shadowText;

    TextViewParam getFrontTextViewParam(NPTextViewParam p) {
        return TextViewParam(p.getString(), p.getFontInfo(), p.getFrontColor());
    }

    TextViewParam getShadowTextViewParam(NPTextViewParam p) {
        return TextViewParam(p.getString(), p.getFontInfo(), p.getShadowColor());
    }

    virtual void beforeDrawProcess(int drawScreen) override {
    
    };

};

