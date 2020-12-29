#include <vsg/all.h>

#include <vsg/platform/unix/Xcb_Window.h>
// #include <xcb/xcb.h>
#include <gdk/gdk.h>
#include <gdk/gdkx.h>
#include <wx/nativewin.h>

class wxVsgWindow : public wxNativeWindow
{
public:
    wxVsgWindow(wxWindow* parent, vsg::ref_ptr<vsg::Viewer> vsgViewer, vsg::ref_ptr<vsg::Window> vsgWindow, GtkWidget* gtk);
    ~wxVsgWindow();

    static wxVsgWindow* create(wxWindow* parent, vsg::ref_ptr<vsg::Viewer> vsgViewer, vsg::ref_ptr<vsg::Window> vsgWindow);

protected:
    vsg::ref_ptr<vsg::Viewer> _vsgViewer;
    vsg::ref_ptr<vsg::Window> _vsgWindow;

    void OnSize(wxSizeEvent& event);
    void OnMove(wxMoveEvent& event);
    void OnPaint(wxPaintEvent& event);
};