#include <vsg/all.h>

#ifdef USE_VSGXCHANGE
#include <vsgXchange/ReaderWriter_all.h>
#endif

#include <iostream>

// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include "wxVsgWindow.h"

bool createVsgWindow(vsg::ref_ptr<vsg::Viewer>& viewer, vsg::ref_ptr<vsg::Window>& window)
{
    auto windowTraits = vsg::WindowTraits::create();
    // windowTraits->windowTitle = "wxSI";
    windowTraits->debugLayer = false;
    windowTraits->apiDumpLayer = false;
    windowTraits->fullscreen = false;
    windowTraits->decoration = false;

    auto scene = vsg::read_cast<vsg::Node>("../../models/teapot.vsgt");

    if (!scene)
    {
        std::cout<<"No scene loaded, please specify 3d model on command line."<<std::endl;
        return false;
    }

    // create the viewer and assign window(s) to it
    viewer = vsg::Viewer::create();

    window = vsg::Window::create(windowTraits);

    if (!window)
    {
        std::cout<<"Could not create windows."<<std::endl;
        return false;
    }

    viewer->addWindow(window);

    // compute the bounds of the scene graph to help position camera
    vsg::ComputeBounds computeBounds;
    scene->accept(computeBounds);
    vsg::dvec3 centre = (computeBounds.bounds.min+computeBounds.bounds.max)*0.5;
    double radius = vsg::length(computeBounds.bounds.max-computeBounds.bounds.min)*0.6;
    double nearFarRatio = 0.0001;

    // set up the camera
    auto lookAt = vsg::LookAt::create(centre+vsg::dvec3(0.0, -radius*3.5, 0.0), centre, vsg::dvec3(0.0, 0.0, 1.0));

    auto perspective = vsg::Perspective::create(30.0, static_cast<double>(window->extent2D().width) / static_cast<double>(window->extent2D().height), nearFarRatio*radius, radius * 4.5);

    auto camera = vsg::Camera::create(perspective, lookAt, vsg::ViewportState::create(window->extent2D()));

    // add close handler to respond to pressing the window close window button and pressing escape
    viewer->addEventHandler(vsg::CloseHandler::create(viewer));

    // add a trackball event handler to control the camera view use the mouse
    viewer->addEventHandler(vsg::Trackball::create(camera));

    // create a command graph to render the scene on specified window
    auto commandGraph = vsg::createCommandGraphForView(window, camera, scene);
    viewer->assignRecordAndSubmitTaskAndPresentation({commandGraph});

    // compile all the the Vulkan objects and transfer data required to render the scene
    viewer->compile();

viewer->advanceToNextFrame();
#if 0
    // rendering main loop
    while (viewer->advanceToNextFrame())
    {
#endif

        // pass any events into EventHandlers assigned to the Viewer
        viewer->handleEvents();
        // if (hudinit) hud->render();

        viewer->update();

        viewer->recordAndSubmit();

        viewer->present();

        // if(!hudinit)
        // {
        //     hud->init();
        //     hudinit = true;
        // }

#if 0
    }
#endif
   
    // clean up done automatically thanks to ref_ptr<>
    return true;
}

class MyApp : public wxApp
{
public:
    virtual bool OnInit();
};

class MyFrame : public wxFrame
{
public:
    MyFrame();
private:
    void OnHello(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
};

enum
{
    ID_Hello = 1
};

wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit()
{
    MyFrame *frame = new MyFrame();
    frame->Show(true);
    return true;
}

MyFrame::MyFrame()
    : wxFrame(NULL, wxID_ANY, "Hello World")
{
    wxMenu *menuFile = new wxMenu;
    menuFile->Append(ID_Hello, "&Hello...\tCtrl-H",
                     "Help string shown in status bar for this menu item");
    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT);
    wxMenu *menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT);
    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&File");
    menuBar->Append(menuHelp, "&Help");
    SetMenuBar( menuBar );
    CreateStatusBar();
    SetStatusText("Welcome to wxWidgets!");
    Bind(wxEVT_MENU, &MyFrame::OnHello, this, ID_Hello);
    Bind(wxEVT_MENU, &MyFrame::OnAbout, this, wxID_ABOUT);
    Bind(wxEVT_MENU, &MyFrame::OnExit, this, wxID_EXIT);

    wxBoxSizer *topsizer = new wxBoxSizer( wxVERTICAL );
    vsg::ref_ptr<vsg::Window> vsgWin;
    vsg::ref_ptr<vsg::Viewer> vsgViewer;

    createVsgWindow(vsgViewer, vsgWin);

    wxVsgWindow* wxVsgWin = wxVsgWindow::create(this, vsgViewer, vsgWin);

    // wxWindow* win = new wxWindow(this,-1, wxDefaultPosition, wxSize(300, 200));
    wxTopLevelWindow* win = new wxTopLevelWindow(this,-1, "Second Window", wxDefaultPosition, wxSize(300, 200));
    win->SetBackgroundColour(wxColour(0, 0, 50));
    win->Show();

    topsizer->Add(wxVsgWin,
        1,            // make vertically stretchable
        wxEXPAND |    // make horizontally stretchable
        wxALL,        //   and make border all around
        10 );         // set border width to 10
    topsizer->Add(win, 1, wxEXPAND | wxALL, 10 );

    SetSizerAndFit(topsizer);

}

void MyFrame::OnExit(wxCommandEvent& event)
{
    Close(true);
}

void MyFrame::OnAbout(wxCommandEvent& event)
{
    wxMessageBox("This is a wxWidgets Hello World example",
                 "About Hello World", wxOK | wxICON_INFORMATION);
}

void MyFrame::OnHello(wxCommandEvent& event)
{
    wxLogMessage("Hello world from wxWidgets!");
}
