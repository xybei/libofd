#include <assert.h>
#include <string>
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include "utils/logger.h"
#include "PackageView.h"
#include "DocumentView.h"

using namespace ofd;

static void on_toolbar_document_cb(GtkWidget *widget, gpointer user_data){
    PackageView *packageView = (PackageView*)user_data;
    if (packageView == nullptr) {
        LOG_WARN("PackageView is NULL");
        return;
    }


    std::string name = gtk_widget_get_name(widget);
    LOG_DEBUG("toolbar clicked. name:%s", name.c_str());

    DocumentViewPtr documentView = packageView->GetCurrentDocumentView();
    if (name != "document-open"){
        if (documentView == nullptr){
            LOG_WARN("DocumentView is NULL");
            return;
        }
    } 

    if (name == "document-open"){
        packageView->CmdFileOpen();
    } else if (name == "document-properties"){
        documentView->CmdFileProperties();
    } else if (name == "document-print"){
        documentView->DoPrint();
    }
}

static void on_toolbar_edit_cb(GtkWidget *widget, gpointer user_data){
    std::string name = gtk_widget_get_name(widget);
    LOG_DEBUG("toolbar clicked. name:%s", name.c_str());
}

static void on_toolbar_go_cb(GtkWidget *widget, gpointer user_data){
    PackageView *packageView = (PackageView*)user_data;
    if (packageView == nullptr) {
        LOG_WARN("PackageView is NULL");
        return;
    }

    DocumentViewPtr documentView = packageView->GetCurrentDocumentView();
    if (documentView == nullptr){
        LOG_WARN("DocumentView is NULL");
        return;
    }

    std::string name = gtk_widget_get_name(widget);
    LOG_DEBUG("toolbar clicked. name:%s", name.c_str());
    if (name == "go-first"){
        documentView->CmdFirstPage();
    } else if (name == "go-previous"){
        documentView->CmdPreviousPage();
    } else if (name == "go-next"){
        documentView->CmdNextPage();
    } else if (name == "go-last"){
        documentView->CmdLastPage();
    } else if (name == "go-jump"){
        documentView->CmdGotoPage();
    }
}

static void on_toolbar_zoom_cb(GtkWidget *widget, gpointer user_data){
    PackageView *packageView = (PackageView*)user_data;
    if (packageView == nullptr) {
        LOG_WARN("PackageView is NULL");
        return;
    }

    DocumentViewPtr documentView = packageView->GetCurrentDocumentView();
    if (documentView == nullptr){
        LOG_WARN("DocumentView is NULL");
        return;
    }

    std::string name = gtk_widget_get_name(widget);
    LOG_DEBUG("toolbar clicked. name:%s", name.c_str());
    if (name == "zoom-in"){
        documentView->CmdZoomIn();
    } else if (name == "zoom-out"){
        documentView->CmdZoomOut();
    } else if (name == "zoom-fit-best"){
        documentView->CmdZoomFitBest();
    } else if (name == "zoom-original"){
        documentView->CmdZoomOriginal();
    }
}

#define TOOL_BUTTON_SIZE GTK_ICON_SIZE_SMALL_TOOLBAR
//#define TOOL_BUTTON_SIZE GTK_ICON_SIZE_DND

GtkWidget* GTK_IMAGE_FROM_RESOURCE(const std::string &iconName){
    //std::string resourcePath = std::string("/icons/gtkofd/faenza/16/") + iconName + ".png";
    //std::string resourcePath = std::string("/icons/gtkofd/faenza/24/") + iconName + ".png";
    //std::string resourcePath = std::string("/icons/gtkofd/faenza/48/") + iconName + ".png";
    std::string resourcePath = std::string("/icons/gtkofd/emerald/16/") + iconName + ".svg";
    //std::string resourcePath = std::string("/icons/gtkofd/emerald/24/") + iconName + ".svg";
    return gtk_image_new_from_resource(resourcePath.c_str());
}

extern PackageViewPtr m_packageView;
static void insert_toolbar_item(GtkToolbar *toolbar, const std::string &name, const std::string &label, GCallback callback, void *user_data=nullptr, bool enable=true, const std::string &tooltip=""){
    user_data = (void*)m_packageView.get();
    GtkToolItem *item = gtk_tool_button_new(GTK_IMAGE_FROM_RESOURCE(name.c_str()), label.c_str());
    if (tooltip.empty()){
        gtk_tool_item_set_tooltip_text(item, label.c_str());
    } else {
        gtk_tool_item_set_tooltip_text(item, tooltip.c_str());
    }
    gtk_toolbar_insert(toolbar, item, -1);
    gtk_widget_set_name(GTK_WIDGET(item), name.c_str());
    gtk_widget_set_sensitive(GTK_WIDGET(item), enable);
    g_signal_connect(G_OBJECT(item), "clicked", G_CALLBACK(callback), user_data);
}

void init_toolbar(GtkToolbar *mainToolbar){
    gtk_toolbar_set_style(GTK_TOOLBAR(mainToolbar), GTK_TOOLBAR_BOTH);
    //gtk_toolbar_set_style(GTK_TOOLBAR(mainToolbar), GTK_TOOLBAR_ICONS);

    //gtk_toolbar_set_icon_size(mainToolbar, GTK_ICON_SIZE_MENU); // 16px for menu
    gtk_toolbar_set_icon_size(mainToolbar, GTK_ICON_SIZE_SMALL_TOOLBAR); // 16px for small toolbar
    //gtk_toolbar_set_icon_size(mainToolbar, GTK_ICON_SIZE_LARGE_TOOLBAR); // 24px for large toolbar
    //gtk_toolbar_set_icon_size(mainToolbar, GTK_ICON_SIZE_DND); // 32px for drang and drop
    //gtk_toolbar_set_icon_size(mainToolbar, GTK_ICON_SIZE_DIALOG); // 48px for dialog

    gtk_container_set_border_width(GTK_CONTAINER(mainToolbar), 0);

    // https://specifications.freedesktop.org/icon-naming-spec/icon-naming-spec-latest.html
    
    //insert_toolbar_item(mainToolbar, "document-new", "新建", G_CALLBACK(on_toolbar_document_cb));
    insert_toolbar_item(mainToolbar, "document-open", "打开", G_CALLBACK(on_toolbar_document_cb));
    insert_toolbar_item(mainToolbar, "document-save", "保存", G_CALLBACK(on_toolbar_document_cb), nullptr, false);
    //insert_toolbar_item(mainToolbar, "document-save-as", "另存", G_CALLBACK(on_toolbar_document_cb));
    insert_toolbar_item(mainToolbar, "document-properties", "属性", G_CALLBACK(on_toolbar_document_cb), nullptr, true, "文档属性");
    insert_toolbar_item(mainToolbar, "document-page-setup", "设置", G_CALLBACK(on_toolbar_document_cb), nullptr, true, "页面设置");
    insert_toolbar_item(mainToolbar, "document-print", "打印", G_CALLBACK(on_toolbar_document_cb));

    // -------- separator tool item --------
    gtk_toolbar_insert(GTK_TOOLBAR(mainToolbar), gtk_separator_tool_item_new(), -1);

    insert_toolbar_item(mainToolbar, "go-first", "首页", G_CALLBACK(on_toolbar_go_cb));
    insert_toolbar_item(mainToolbar, "go-previous", "上页", G_CALLBACK(on_toolbar_go_cb));
    insert_toolbar_item(mainToolbar, "go-next", "下页", G_CALLBACK(on_toolbar_go_cb));
    insert_toolbar_item(mainToolbar, "go-last", "末页", G_CALLBACK(on_toolbar_go_cb));
    insert_toolbar_item(mainToolbar, "go-jump", "跳转", G_CALLBACK(on_toolbar_go_cb));

    // -------- separator tool item --------
    gtk_toolbar_insert(GTK_TOOLBAR(mainToolbar), gtk_separator_tool_item_new(), -1);

    insert_toolbar_item(mainToolbar, "zoom-in", "放大", G_CALLBACK(on_toolbar_zoom_cb));
    insert_toolbar_item(mainToolbar, "zoom-out", "缩小", G_CALLBACK(on_toolbar_zoom_cb));
    insert_toolbar_item(mainToolbar, "zoom-fit-best", "适合", G_CALLBACK(on_toolbar_zoom_cb));
    insert_toolbar_item(mainToolbar, "zoom-original", "原始", G_CALLBACK(on_toolbar_zoom_cb));

    // -------- separator tool item --------
    gtk_toolbar_insert(GTK_TOOLBAR(mainToolbar), gtk_separator_tool_item_new(), -1);

    insert_toolbar_item(mainToolbar, "edit-copy", "拷贝", G_CALLBACK(on_toolbar_edit_cb));
    insert_toolbar_item(mainToolbar, "edit-find", "查找", G_CALLBACK(on_toolbar_edit_cb));

    // -------- separator tool item --------
    gtk_toolbar_insert(GTK_TOOLBAR(mainToolbar), gtk_separator_tool_item_new(), -1);

    insert_toolbar_item(mainToolbar, "application-exit", "退出", G_CALLBACK(gtk_main_quit));
}
