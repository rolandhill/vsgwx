#include <stdio.h>
#include "wxVsgWindow.h"
#include <gtk/gtk.h>
#include <gdk/gdkx.h>

static void my_gtk_realize(GtkWidget* widget, gpointer user)
{
    gtk_widget_set_window(widget, (GdkWindow*)user);
}

wxVsgWindow* wxVsgWindow::create(wxWindow* parent, vsg::ref_ptr<vsg::Viewer> vsgViewer, vsg::ref_ptr<vsg::Window> vsgWindow)
{
    xcb_window_t nativeHandle = std::any_cast<xcb_window_t>(vsgWindow->traits()->nativeWindow);
    GdkWindow* gdkWin = gdk_x11_window_foreign_new_for_display (gdk_display_get_default(), nativeHandle);
    GtkWidget* gtk = gtk_widget_new(GTK_TYPE_WINDOW, NULL);
    g_signal_connect(gtk, "realize", G_CALLBACK(my_gtk_realize), gdkWin);
    gtk_widget_set_has_window(gtk, TRUE);
    gtk_widget_realize(gtk);

    gtk_widget_show_all(gtk);

    return new wxVsgWindow(parent, vsgViewer, vsgWindow, gtk);
}

wxVsgWindow::wxVsgWindow(wxWindow* parent, vsg::ref_ptr<vsg::Viewer> vsgViewer, vsg::ref_ptr<vsg::Window> vsgWindow, GtkWidget* gtk) :
    wxNativeWindow(parent, wxID_ANY, gtk),
    // wxNativeContainerWindow(std::any_cast<xcb_window_t>(vsgWindow->traits()->nativeWindow)),
    _vsgViewer(vsgViewer),
    _vsgWindow(vsgWindow)
{
    // auto traits = _vsgWindow->traits();
    // SetSize(wxSize(traits->width, traits->height));
    // SetPosition(wxPoint(traits->x, traits->y));

    // Bind(wxEVT_SIZE, &wxVsgWindow::OnSize, this);
    // parent->Bind(wxEVT_MOVE, &wxVsgWindow::OnMove, this);
    // Bind(wxEVT_PAINT, &wxVsgWindow::OnPaint, this);
}

wxVsgWindow::~wxVsgWindow()
{

}

void wxVsgWindow::OnSize(wxSizeEvent& event)
{
    wxSize size = event.GetSize();
    const uint32_t values[] = { (uint32_t)(size.x * 1.2), (uint32_t)(size.y * 1.2)};
    std::cout << "Resize Window: " << size.x << "  " << size.y << std::endl;
    xcb_configure_window (std::any_cast<xcb_connection_t*>(_vsgWindow->traits()->systemConnection), std::any_cast<xcb_window_t>(_vsgWindow->traits()->nativeWindow), XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT, values);
    xcb_flush(std::any_cast<xcb_connection_t*>(_vsgWindow->traits()->systemConnection));
}

void wxVsgWindow::OnMove(wxMoveEvent& event)
{
    wxPoint pt = event.GetPosition();
    const uint32_t values[] = { (uint32_t)pt.x, (uint32_t)pt.y };
    std::cout << "Move Window: " << pt.x << "  " << pt.y << std::endl;
    xcb_configure_window (std::any_cast<xcb_connection_t*>(_vsgWindow->traits()->systemConnection), std::any_cast<xcb_window_t>(_vsgWindow->traits()->nativeWindow), XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y, values);
    xcb_flush(std::any_cast<xcb_connection_t*>(_vsgWindow->traits()->systemConnection));
}

void wxVsgWindow::OnPaint( wxPaintEvent& WXUNUSED(event) )
{
    _vsgViewer->advanceToNextFrame();

    _vsgViewer->handleEvents();

    _vsgViewer->update();

    _vsgViewer->recordAndSubmit();

    _vsgViewer->present();
}
