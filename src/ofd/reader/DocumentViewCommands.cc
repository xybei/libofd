#include <assert.h>
#include <string>
#include <algorithm>
//#include <gtk/gtk.h>
//#include <gdk/gdk.h>
#include "ofd/Package.h"
#include "utils/logger.h"
#include "ofd/reader/DocumentView.h"

void DocumentView::CmdFileSave(){
}

void DocumentView::CmdFileSaveAs(){
}

void DocumentView::CmdFileExport(){
}

void DocumentView::CmdFilePrint(){
    DoPrint();
}

void DocumentView::CmdFileProperties(){
    //GtkWidget *dlg = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    //gtk_widget_set_size_request(dlg, 800, 600);

    //GtkWidget *notebook = gtk_notebook_new();
    //gtk_container_add(GTK_CONTAINER(dlg), notebook);
    //gtk_notebook_set_tab_pos(GTK_NOTEBOOK(notebook), GTK_POS_TOP);

    //GtkWidget *hbox = gtk_hbox_new(true, 5);
    //gtk_container_add(GTK_CONTAINER(dlg), hbox);

    //GtkWidget *label = gtk_label_new("文档元数据");
    //gtk_container_add(GTK_CONTAINER(hbox), 

    //gobject_unref(dlg);
}

void DocumentView::CmdFileProperties_from_resource(){

}

void DocumentView::CmdFirstPage(){
    FirstPage();
    RedrawDocumentView();
}

void DocumentView::CmdLastPage(){
    LastPage();
    RedrawDocumentView();
}

void DocumentView::CmdNextPage(){
    NextPage();
    RedrawDocumentView();
}

void DocumentView::CmdPreviousPage(){
    PreviousPage();
    RedrawDocumentView();
}

void DocumentView::CmdGotoPage(){
    RedrawDocumentView();
}

void DocumentView::CmdZoomIn(){
    //ZoomIn();
    //RedrawDocumentView();
    ScrollIn();
}

void DocumentView::CmdZoomOut(){
    //ZoomOut();
    //RedrawDocumentView();
    ScrollOut();
}

void DocumentView::CmdZoomFitBest(){
    ZoomFitBest();
    RedrawDocumentView();
}

void DocumentView::CmdZoomOriginal(){
    ZoomOriginal();
    RedrawDocumentView();
}

void DocumentView::CmdMoveUp(){
    //MoveUp();
    //RedrawDocumentView();
    ScrollUp();
}

void DocumentView::CmdMoveDown(){
    //MoveDown();
    //RedrawDocumentView();
    ScrollDown();
}

void DocumentView::CmdMoveLeft(){
    //MoveLeft();
    //RedrawDocumentView();
    ScrollLeft();
}

void DocumentView::CmdMoveRight(){
    //MoveRight();
    //RedrawDocumentView();
    ScrollRight();
}


// ==================== Tools Submenu ====================

void DocumentView::CmdToolsNormal(){
    if (m_action->GetType() != Action::Type::NORMAL){
        m_action = ActionFactory::CreateAction(Action::Type::NORMAL);
    }
}

void DocumentView::CmdToolsSelectAnnotation(){
    if (m_action->GetType() != Action::Type::SELECTANNOTATION){
        m_action = ActionFactory::CreateAction(Action::Type::SELECTANNOTATION);
    }
}

void DocumentView::CmdToolsSelectText(){
    if (m_action->GetType() != Action::Type::SELECTTEXT){
        m_action = ActionFactory::CreateAction(Action::Type::SELECTTEXT);
    }
}

void DocumentView::CmdToolsSnapshot(){
    if (m_action->GetType() != Action::Type::SNAPSHOT){
        m_action = ActionFactory::CreateAction(Action::Type::SNAPSHOT);
    }
}

void DocumentView::CmdToolsDrawLine(){
    if (m_action->GetType() != Action::Type::DRAWLINE){
        m_action = ActionFactory::CreateAction(Action::Type::DRAWLINE);
    }
}

void DocumentView::CmdToolsDrawRect(){
    if (m_action->GetType() != Action::Type::DRAWRECT){
        m_action = ActionFactory::CreateAction(Action::Type::DRAWRECT);
    }
}

void DocumentView::CmdToolsDrawPolyline(){
    if (m_action->GetType() != Action::Type::DRAWPOLYLINE){
        m_action = ActionFactory::CreateAction(Action::Type::DRAWPOLYLINE);
    }
}