#include <RGame.h>
#include <RWindow.h>
#include <rsc/RTexture.h>
#include <rsc/RImage.h>
#include <RMath.h>
#include <RTimer.h>

#include "model.h"

using namespace Redopera;

class View
{
public:
    View():
        node("View", this),
        model_(4096, 4096)
    {
        node.setTransformFunc([this](RNode *sender, const RRect &info){ transform(sender, info); });
        node.setProcessFunc([this](RNode *sender, RNode::Instructs* ins){ process(sender, ins); });
        node.setUpdateFunc([this](RRenderSys *sys){ update(sys); });

        /*滑翔机
        model_.setValue(model_.WIDTH / 2, model_.HEIGHT / 2, 1);
        model_.setValue(model_.WIDTH / 2 - 1, model_.HEIGHT / 2, 1);
        model_.setValue(model_.WIDTH / 2, model_.HEIGHT / 2 + 1, 1);
        model_.setValue(model_.WIDTH / 2 + 1, model_.HEIGHT / 2 + 1, 1);
        model_.setValue(model_.WIDTH / 2 + 1, model_.HEIGHT / 2 - 1, 1);
        */

        model_.setRangeValue(model_.WIDTH / 2, model_.HEIGHT / 2, 3, 8, 1);
        model_.setValue(model_.WIDTH / 2 + 1, model_.HEIGHT / 2 + 1, 0);
        model_.setValue(model_.WIDTH / 2 + 1, model_.HEIGHT / 2 + 6, 0);
    }

    RNode node;

private:
    void update(RRenderSys *sys)
    {
        ++count_;
        if(timer_.elapsed() > 1000)
        {
            rDebug << count_ / (timer_.elapsed() / 1000.);
            timer_.start();
            count_ = 0;
        }

        RRPI rpi = sys->shaders()->use();
        sys->bindVAO();
        tex_.bind();
        sys->drawPlane();
    }

    void transform(RNode *sender, const RRect& info)
    {
        RRect rect(0, 0, std::min(info.width() / gridSize_ + 1ul, model_.WIDTH),
                   std::min(info.height() / gridSize_ + 1ul, model_.HEIGHT));
        rect.setCenter(RRect(RPoint(), model_.size()).center());
        pos_ = rect.pos();

        if(rect.size() != tex_.size())
        {
            tex_.load(nullptr, rect, 3);
            loader_.load(nullptr, rect, 3);
        }

        canvas.setSize(rect.size() * gridSize_);
        canvas.setCenter(info.center());

        RRenderSys * sys = sender->holder<RWindow>()->renderSys();
        RRPI inter = sys->shaders()->use();
        inter.setUniformMat(sys->modelLocal(), Math::modelMat(canvas));
    }

    void process(RNode*, RNode::Instructs*)
    {
        model_.update();

        for(int i = 0; i < loader_.height(); ++i)
        {
            for(int j = 0; j < loader_.width(); ++j)
            {
                loader_.data()[i * loader_.width() * 3 + j * 3] = model_.value(j + pos_.x(), i + pos_.y()) ? 255 : 0;
                loader_.data()[i * loader_.width() * 3 + j * 3 + 1] = model_.value(j + pos_.x(), i + pos_.y()) ? 255 : 0;
                loader_.data()[i * loader_.width() * 3 + j * 3 + 2] = model_.value(j + pos_.x(), i + pos_.y()) ? 255 : 0;
            }
        }
        tex_.reload(loader_.data());
    }

    RTimer timer_;
    unsigned count_ = 0;

    unsigned gridSize_ = 4;
    RRect canvas;
    RPoint2 pos_;

    RImage loader_;
    RTexture tex_;
    Model<int> model_;
};

int main()
{
    RTexture::setDefaultTextureFomat(RTexture::Nearest3);

    RGame game;
    RWindow::Format format;
    format.versionMajor = 4;
    format.versionMinor = 5;
    format.maximization = true;
    format.background = 0x888888ff;
    RWindow window(800, 540, "CA", format);

    View view;
    view.node.changeParent(&window.node);

    window.show();
    return window.node.exec();

    return 0;
}
