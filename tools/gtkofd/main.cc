#include <stdlib.h>
#include <iostream>
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gdk/gdkscreen.h>
#include <assert.h>

#include "ofd/Package.h"
#include "ofd/Document.h"
#include "ofd/Page.h"
#include "utils/logger.h"
#include "utils/utils.h"
#include "ofd/CairoRender.h"
using namespace ofd;

extern "C"{
#include "gtkofd_resources.h"
}
#include "OFDRender.h"
#include "PageWall.h"
#include "ReadWindow.h"


ReadWindowPtr m_readWindow = nullptr;

typedef struct{
    GtkApplicationWindow parentWindow;

    GtkWidget *drawingArea = nullptr;
    GtkWidget *infobar = nullptr;
    GtkWidget *message = nullptr;
    GtkWidget *status = nullptr;
    GMenuModel *mainMenu = nullptr;

    int width = 0;
    int heigth = 0;
    bool isMaximized = false;
    bool isFullscreen = false;

} ApplicationWindowContext;


static void activate_about(GSimpleAction *action, GVariant *parameter, gpointer user_data){

    GtkApplication *app = (GtkApplication*)user_data;
    const gchar *authors[] = {
        "Jiangwen Su <idleuncle@gmail.com>",
        nullptr
    };

    gtk_show_about_dialog(GTK_WINDOW(gtk_application_get_active_window(app)),
            "program-name", "OFD 阅读器",
            "version", g_strdup_printf ("%s,\n基于GTK+ %d.%d.%d",
                "1.0.0",
                gtk_get_major_version (),
                gtk_get_minor_version (),
                gtk_get_micro_version ()),
            "copyright", "(C) 2016-2017 The LastZ Team",
            //"license-type", GTK_LICENSE_LGPL_2_1,
            "license-type", GTK_LICENSE_MIT_X11,
            "website", "http://www.lastz.org",
            "comments", "阅读OFD文件。", 
            "authors", authors,
            "logo-icon-name", "gtk3-demo",
            "title", "关于 OFD 阅读器",
            nullptr);
}

static void activate_quit (GSimpleAction *action, GVariant *parameter, gpointer user_data){

    GtkApplication *app = (GtkApplication*)user_data;
    GtkWidget *win;
    GList *list;

    list = gtk_application_get_windows(app);
    while (list) {
        win = GTK_WIDGET(list->data);
        GList *next = list->next;

        gtk_widget_destroy(win);

        list = next;
    }
}

__attribute__((unused)) static void activate_run(GSimpleAction *action, GVariant *parameter, gpointer user_data){

}

// https://developer.gnome.org/gtk3/3.2/GtkWidget.html#Signal-Details

static gboolean key_press_cb(GtkWidget *widget, GdkEventKey *event, gpointer user_data){
    assert(m_readWindow != nullptr);

    switch (event->keyval){
    case GDK_KEY_i:
        m_readWindow->CmdZoomIn();    
        break;
    case GDK_KEY_o:
        if (!(event->state & GDK_CONTROL_MASK)){
            m_readWindow->CmdZoomOut();    
        }
        break;
    case GDK_KEY_f:
        m_readWindow->CmdZoomFitBest();
        break;
    case GDK_KEY_h:
        m_readWindow->CmdMoveLeft();
        break;
    case GDK_KEY_j:
        m_readWindow->CmdMoveDown();
        break;
    case GDK_KEY_k:
        m_readWindow->CmdMoveUp();
        break;
    case GDK_KEY_l:
        m_readWindow->CmdMoveRight();
        break;
    case GDK_KEY_n:
        m_readWindow->CmdNextPage();
        break;
    case GDK_KEY_p:
        m_readWindow->CmdPreviousPage();
        break;
    default:
        //LOG(DEBUG) << "Key pressed. keyval:" << event->keyval;
        return false;
    }; // switch

    return true;
}

static gboolean key_release_cb(GtkWidget *widget, GdkEventKey *event, gpointer user_data){
    switch (event->keyval){
    case GDK_KEY_o:
        if (event->state & GDK_CONTROL_MASK){
            LOG_DEBUG("Key Ctrl+o released. keyval: 0x%x", event->keyval);
            m_readWindow->CmdFileOpen();
        }
        break;
    case GDK_KEY_plus:
        if (event->state & GDK_CONTROL_MASK){
            // 放大 Ctrl + +
            m_readWindow->CmdZoomIn();
            LOG_DEBUG("Key Ctrl++ released. keyval: 0x%x", event->keyval);
        }
        break;
    case GDK_KEY_minus:
        if (event->state & GDK_CONTROL_MASK){
            // 缩小 Ctrl + -
            m_readWindow->CmdZoomIn();
            LOG_DEBUG("Key Ctrl+- released. keyval: 0x%x", event->keyval);
        }
        break;
    case GDK_KEY_1:
        if (event->state & GDK_CONTROL_MASK){
            // 原始大小 Ctrl + 1
            m_readWindow->CmdZoomOriginal();
            LOG_DEBUG("Key Ctrl+1 released. keyval: 0x%x", event->keyval);
        }
        break;
    case GDK_KEY_2:
        if (event->state & GDK_CONTROL_MASK){
            // 适合页面 Ctrl + 2
            m_readWindow->CmdZoomFitBest();
            LOG_DEBUG("Key Ctrl+2 released. keyval: 0x%x", event->keyval);
        }
        break;
    case GDK_KEY_3:
        if (event->state & GDK_CONTROL_MASK){
            // 适合宽度 Ctrl + 3
            LOG_DEBUG("Key Ctrl+3 released. keyval: 0x%x", event->keyval);
        }
        break;
    case GDK_KEY_4:
        if (event->state & GDK_CONTROL_MASK){
            // 适合高度 Ctrl + 4 
            LOG_DEBUG("Key Ctrl+4 released. keyval: 0x%x", event->keyval);
        }
        break;
    case GDK_KEY_Home:
        m_readWindow->CmdFirstPage();
        break;
    case GDK_KEY_End:
        m_readWindow->CmdLastPage();
        break;
    case GDK_KEY_Page_Down:
        LOG_DEBUG("%s", "Page Down KEY RELEASED!");
        m_readWindow->CmdNextPage();
        break;
    case GDK_KEY_Page_Up:
        LOG_DEBUG("%s", "Page Up KEY RELEASED!");
        m_readWindow->CmdPreviousPage();
        break;
    case GDK_KEY_Down:
        LOG_DEBUG("%s", "Down KEY RELEASED!");
        m_readWindow->CmdMoveDown();
        break;
    case GDK_KEY_Up:
        LOG_DEBUG("%s", "Up KEY RELEASED!");
        m_readWindow->CmdMoveUp();
        break;
    case GDK_KEY_Left:
        LOG_DEBUG("%s", "Left KEY RELEASED!");
        m_readWindow->CmdMoveLeft();
        break;
    case GDK_KEY_Right:
        LOG_DEBUG("%s", "Right KEY RELEASED!");
        m_readWindow->CmdMoveRight();
        break;
    case GDK_KEY_space:
        LOG_DEBUG("%s", "SPACE KEY RELEASED!");
        break;
    default:
        //LOG_DEBUG)("Key released. keyval: 0x%x", event->keyval);
        return false;
    }; // switch

    return true;
}
/*
struct GdkEventButton {
  GdkEventType type;
  GdkWindow *window;
  gint8 send_event;
  guint32 time;
  gdouble x;
  gdouble y;
  gdouble *axes;
  guint state;
  guint button;
  GdkDevice *device;
  gdouble x_root, y_root;
};
 */
static gboolean button_press_cb(GtkWidget *widget, GdkEventButton *event, gpointer user_data){
    if (event->button == GDK_BUTTON_PRIMARY){
        //LOG_DEBUG("LEFT BUTTON PRESSED! x:%d y:%d", event->x, event->y);
        //next_page();
    } else if (event->button == GDK_BUTTON_SECONDARY){
        LOG_DEBUG("RIGHT BUTTON PRESSED! x:%d y:%d", event->x, event->y);
        //prev_page();
    }

    return false;
}

__attribute__((unused)) static gboolean button_release_cb(GtkWidget *widget, GdkEventButton *event, gpointer user_data){
    return false;
}

/*
struct GdkEventConfigure {
  GdkEventType type;
  GdkWindow *window;
  gint8 send_event;
  gint x, y;
  gint width;
  gint height;
};
 */
__attribute__((unused)) static gboolean configure_event_cb(GtkWidget *widget, GdkEventConfigure *event, gpointer user_data){
    return false;
}

static gboolean motion_notify_cb(GtkWidget *widget, GdkEventMotion *event, gpointer user_data){
    int x, y;
    GdkModifierType state;

    gdk_window_get_device_position(event->window, event->device, &x, &y, &state);

    //if (state & GDK_BUTTON1_MASK){
        //LOG(DEBUG) << "MOTION NOTIFY! x:" << x << " y:" << y;
    //}
    return true;
}

/*
struct GdkEventScroll {
  GdkEventType type;
  GdkWindow *window;
  gint8 send_event;
  guint32 time;
  gdouble x;
  gdouble y;
  guint state;
  GdkScrollDirection direction;
  GdkDevice *device;
  gdouble x_root, y_root;
  gdouble delta_x;
  gdouble delta_y;
  guint is_stop : 1;
};
 */
__attribute__((unused)) gboolean scroll_event_cb(GtkWidget *widget, GdkEventScroll *event, gpointer user_data){
    //GdkScrollDirection direction = event->direction;
    //LOG(DEBUG) << "Scroll Event. Delta x:" << event->delta_x << " Delta y:" << event->delta_y << " Direction:" << direction;

    if (event->state & GDK_SHIFT_MASK){
        if (event->direction == GDK_SCROLL_UP){
            m_readWindow->CmdZoomIn();
        } else if (event->direction == GDK_SCROLL_DOWN){
            m_readWindow->CmdZoomOut();
        }
    } else {
        if (event->direction == GDK_SCROLL_UP){
            m_readWindow->CmdMoveUp();
        } else if (event->direction == GDK_SCROLL_DOWN){
            m_readWindow->CmdMoveDown();
        } else if (event->direction == GDK_SCROLL_LEFT){
            m_readWindow->CmdMoveLeft();
        } else if (event->direction == GDK_SCROLL_RIGHT){
            m_readWindow->CmdMoveRight();
        }
    }

    return false;
}

/*
struct GdkEventCrossing {
  GdkEventType type;
  GdkWindow *window;
  gint8 send_event;
  GdkWindow *subwindow;
  guint32 time;
  gdouble x;
  gdouble y;
  gdouble x_root;
  gdouble y_root;
  GdkCrossingMode mode;
  GdkNotifyType detail;
  gboolean focus;
  guint state;
};
 */
__attribute__((unused)) static gboolean enter_notify_cb(GtkWidget *widget, GdkEventCrossing *event, gpointer user_data){
    LOG_DEBUG("ENTER NOTIFY x:%d y:%d", event->x, event->y);
    return false;
}

__attribute__((unused)) static gboolean leave_notify_cb(GtkWidget *widget, GdkEventCrossing *event, gpointer user_data){
    LOG_DEBUG("LEAVE NOTIFY x:%d y:%d", event->x, event->y);
    return false;
}

/*
typedef enum {
  GTK_DIR_TAB_FORWARD,
  GTK_DIR_TAB_BACKWARD,
  GTK_DIR_UP,
  GTK_DIR_DOWN,
  GTK_DIR_LEFT,
  GTK_DIR_RIGHT
} GtkDirectionType;
 */
__attribute__((unused)) static gboolean focus_cb(GtkWidget *widget, GtkDirectionType direction, gpointer user_data){
    LOG_DEBUG("FOCUS direction:%d", direction);
    return false;
}

/*
struct GdkEventFocus {
  GdkEventType type;
  GdkWindow *window;
  gint8 send_event;
  gint16 in;
};
 */
__attribute__((unused)) static gboolean focus_in_event_cb(GtkWidget *widget, GdkEventFocus *event, gpointer user_data){
    LOG_DEBUG("Keyboard FOCUS in EVENT. Gained keyboard focus:%d", event->in);
    return false;
}

__attribute__((unused)) static gboolean focus_out_event_cb(GtkWidget *widget, GdkEventFocus *event, gpointer user_data){
    LOG_DEBUG("Keyboard FOCUS out EVENT. Gained keyboard focus:%d", event->in);
    return false;
}

static void size_allocate_cb(GtkWidget *widget, GdkRectangle *allocation, gpointer user_data){

    assert(m_readWindow != nullptr);
    m_readWindow->OnSize(allocation->width, allocation->height);

}

static gint draw_cb(GtkWindow *widget, cairo_t *cr, gpointer data){

    m_readWindow->OnDraw(cr);

    return true;
}

__attribute__((unused)) static void showInfoBar(GSimpleAction *action, GVariant *parameter, gpointer data){

    //const gchar *name = g_action_get_name(G_ACTION(action));
    //const gchar *value = g_variant_get_string(parameter, nullptr);

    //gchar *text = g_strdup_printf("Your activated action: \"%s\".\n"
            //"Current value: %s", name ,value);
    //gtk_label_set_text(GTK_LABEL(message), text);
    //gtk_widget_show(infobar);

    //g_free(text);
}

void adjust_window_size(GtkWindow *window){

    GdkRectangle workArea;

    GdkDisplay *display = gdk_display_get_default();
    GdkMonitor *monitor = gdk_display_get_primary_monitor(display);
    gdk_monitor_get_workarea(monitor, &workArea);

    //GdkScreen *screen = gdk_screen_get_default();
    //gint monitor_num = gdk_screen_get_primary_monitor(screen);
    //gdk_screen_get_monitor_workarea(screen, monitor_num, &workArea);


    GtkRequisition minimum_size;
    GtkRequisition natural_size;
    gtk_widget_get_preferred_size(GTK_WIDGET(window), &minimum_size, &natural_size);

    gtk_window_set_default_size(window, natural_size.width, workArea.height);
    //gtk_window_set_default_size(window, natural_size.width, natural_size.height);
    //gtk_window_set_position(window, GTK_WIN_POS_CENTER);
}

void app_set_theme_from_file(const gchar *theme_path) {
    static GtkCssProvider *provider = NULL;
    GFile *file;
    GdkScreen *screen;
    screen = gdk_screen_get_default();
    if(theme_path!=NULL) {
        file = g_file_new_for_path(theme_path);
        g_print("Loading GTK3 CSS File: %s\n", theme_path);
        if(file!=NULL)
        {
            if(provider==NULL)
                provider = gtk_css_provider_new();
            gtk_css_provider_load_from_file(provider, file, NULL);
            gtk_style_context_add_provider_for_screen(screen,
                GTK_STYLE_PROVIDER(provider),
                GTK_STYLE_PROVIDER_PRIORITY_USER);
            gtk_style_context_reset_widgets(screen);
            g_print("Loaded GTK3 CSS File Successfully!\n");
        }
    }
    else
    {
        if(provider!=NULL)
        {
            gtk_style_context_remove_provider_for_screen(screen,
                GTK_STYLE_PROVIDER(provider));
            g_object_unref(provider);
            provider = NULL;
        }
        gtk_style_context_reset_widgets(screen);
    }
}

void app_set_theme_from_resource(const gchar *theme_path) {
    static GtkCssProvider *provider = NULL;
    GdkScreen *screen;
    screen = gdk_screen_get_default();
    if(theme_path!=NULL) {
        g_print("Loading GTK3 CSS File: %s\n", theme_path);
        if(provider==NULL)
            provider = gtk_css_provider_new();
        gtk_css_provider_load_from_resource(provider, theme_path);
        gtk_style_context_add_provider_for_screen(screen,
            GTK_STYLE_PROVIDER(provider),
            GTK_STYLE_PROVIDER_PRIORITY_USER);
        gtk_style_context_reset_widgets(screen);
        g_print("Loaded GTK3 CSS File Successfully!\n");
    }
    else
    {
        if(provider!=NULL)
        {
            gtk_style_context_remove_provider_for_screen(screen,
                GTK_STYLE_PROVIDER(provider));
            g_object_unref(provider);
            provider = NULL;
        }
        gtk_style_context_reset_widgets(screen);
    }
}

// defined in toolbar.cc
void init_toolbar(GtkToolbar *mainToolbar);

static void activate(GApplication *app){

    // -------- Load widgets from resource by GtkBuilder --------
    //GtkBuilder *builder = gtk_builder_new_from_file("./app.ui");
    GtkBuilder *builder = gtk_builder_new_from_resource("/ui/app.ui");

    // -------- mainWindow --------
    GtkWindow *mainWindow = (GtkWindow*)gtk_builder_get_object(builder, "mainWindow");
    assert(mainWindow != nullptr);
    g_signal_connect_swapped(G_OBJECT(mainWindow),"destroy",G_CALLBACK(gtk_main_quit), nullptr);
    m_readWindow->m_mainWindow = mainWindow;

    // -------- mainToolbar --------
    GtkToolbar *mainToolbar = (GtkToolbar*)gtk_builder_get_object(builder, "mainToolbar");
    assert(mainToolbar != nullptr);
    m_readWindow->m_mainToolbar = mainToolbar;

    init_toolbar(mainToolbar);

    // -------- infobar --------
    GtkWidget *infobar = GTK_WIDGET(gtk_builder_get_object(builder, "infobar"));
    assert(infobar != nullptr);
    m_readWindow->infobar = infobar;

    // -------- message --------
    GtkWidget *message = GTK_WIDGET(gtk_builder_get_object(builder, "message"));
    assert(message != nullptr);
    m_readWindow->message = message;

    // -------- drawingArea --------
    GtkWidget *drawingArea = GTK_WIDGET(gtk_builder_get_object(builder, "drawingArea"));
    assert(drawingArea != nullptr);
    m_readWindow->drawingArea = drawingArea;

    // -------- renderingWindow --------
    GtkWindow *renderingWindow = (GtkWindow*)gtk_builder_get_object(builder, "renderingWindow");
    assert(renderingWindow != nullptr);
    m_readWindow->m_renderingWindow = renderingWindow;

    g_object_unref(builder);

    // -------- Widgets callbacks --------

    static GActionEntry win_entries[] = {
        //{ "run", activate_run, nullptr, nullptr, nullptr },
        { "bold", activate_about, nullptr, nullptr, nullptr },
    };
    g_action_map_add_action_entries(G_ACTION_MAP(mainWindow),
            win_entries, G_N_ELEMENTS(win_entries), mainWindow);
    g_signal_connect(drawingArea, "draw", G_CALLBACK(draw_cb), nullptr);
    g_signal_connect(drawingArea, "size-allocate", G_CALLBACK(size_allocate_cb), nullptr);

    gtk_widget_hide(GTK_WIDGET(mainToolbar));

    // -------- Drawing Widget --------
    GtkWidget *drawingWidget = GTK_WIDGET(drawingArea);

    g_signal_connect(G_OBJECT(mainWindow), "key-press-event", G_CALLBACK(key_press_cb), nullptr);
    g_signal_connect(G_OBJECT(mainWindow), "key-release-event", G_CALLBACK(key_release_cb), nullptr);

    g_signal_connect(G_OBJECT(drawingWidget), "button-press-event", G_CALLBACK(button_press_cb), nullptr);
    g_signal_connect(G_OBJECT(drawingWidget), "motion-notify-event", G_CALLBACK(motion_notify_cb), nullptr);
    //g_signal_connect(G_OBJECT(drawingWidget), "enter-notify-event", G_CALLBACK(enter_notify_cb), nullptr);
    //g_signal_connect(G_OBJECT(drawingWidget), "leave-notify-event", G_CALLBACK(leave_notify_cb), nullptr);
    g_signal_connect(G_OBJECT(drawingWidget), "focus", G_CALLBACK(focus_cb), nullptr);
    g_signal_connect(G_OBJECT(drawingWidget), "focus-in-event", G_CALLBACK(focus_in_event_cb), nullptr);
    g_signal_connect(G_OBJECT(drawingWidget), "focus-out-event", G_CALLBACK(focus_out_event_cb), nullptr);

    g_signal_connect(G_OBJECT(drawingWidget), "scroll-event", G_CALLBACK(scroll_event_cb), nullptr);

    gtk_widget_set_events(drawingWidget, gtk_widget_get_events(drawingWidget)
            | GDK_KEY_PRESS_MASK
            | GDK_ENTER_NOTIFY_MASK
            | GDK_LEAVE_NOTIFY_MASK
            | GDK_BUTTON_PRESS_MASK
            | GDK_SCROLL_MASK
            | GDK_POINTER_MOTION_MASK
            | GDK_POINTER_MOTION_HINT_MASK);

    // -------- Main Window --------
    gtk_window_set_title(mainWindow, "OFD 阅读器");
    // Hide the title bar and the board.
    //gtk_window_set_decorated(mainWindow, false);
    adjust_window_size(mainWindow);
    gtk_window_activate_focus(mainWindow);


    //gtk_window_set_opacity(mainWindow, 0.85);
    //gtk_window_set_focus_on_map(mainWindow, true);

    //gtk_window_set_icon(mainWindow, create_pixbuf("./app.png"));

    gtk_application_add_window(GTK_APPLICATION(app), mainWindow);
    gtk_widget_show_all(GTK_WIDGET(mainWindow));

    gtk_widget_hide(infobar);

    //app_set_theme_from_resource("/themes/Adwaita/gtk-3.22/gtk-dark.css");
    //app_set_theme_from_resource("/themes/Adwaita/gtk-3.22/gtk-light.css");

    // -------- United-Ubuntu --------
    //app_set_theme_from_resource("/themes/United-Ubuntu/gtk-3.0/gtk.css");
    //app_set_theme_from_resource("/themes/United-Ubuntu/gtk-3.0/gtk-dark.css");

    // -------- Gnome OSX-III-1.0 --------
    //app_set_theme_from_resource("/themes/Gnome OSX-III-1.0/gtk-3.0/gtk.css");
    //app_set_theme_from_resource("/themes/Gnome OSX-III-1.0/gtk-3.0/gtk-dark.css");

    // -------- Minwaita-OSX --------
    //app_set_theme_from_resource("/themes/Minwaita-OSX-Dark/gtk-3.0/gtk.css");
    //app_set_theme_from_resource("/themes/Minwaita-OSX/gtk-3.0/gtk.css");
    //app_set_theme_from_resource("/themes/Minwaita-OSX/gtk-3.0/gtk-dark.css");

    // -------- OSX-Arc-White --------
    app_set_theme_from_resource("/themes/OSX-Arc-White/gtk-3.22/gtk-light.css");
    //app_set_theme_from_resource("/themes/OSX-Arc-White/gtk-3.22/gtk-dark.css");

    //app_set_theme_from_resource("/themes/OSX-Arc-White/gtk-3.22/gtk-darker.css");

    //app_set_theme("/themes/OSX-Arc-White/gtk-3.22/gtk-light.css");
    ////app_set_theme_from_file("/themes/OSX-Arc-White/gtk-3.22/gtk-darker.css");
    //app_set_theme("./themes/OSX-Arc-White/gtk-3.22/gtk-dark.css");
    //app_set_theme("./themes/OSX-Arc-White/gtk-3.22/gtk-solid-dark.css");
    //app_set_theme("./themes/OSX-Arc-White/gtk-3.22/gtk-solid-darker.css");
    //app_set_theme("./themes/OSX-Arc-White/gtk-3.22/gtk-solid.css");
    
    //app_set_theme("./themes/United-Ubuntu-Dark/gtk-3.0/gtk.css");
    
    //app_set_theme("./themes/Vimix-Theme/VimixLight/gtk-3.0/gtk-dark.css");
    //app_set_theme("./themes/Vimix-Theme/VimixDark-Beryl/gtk-3.0/gtk-dark.css");
    //app_set_theme("./themes/Vimix-Theme/VimixDark-Beryl/gtk-3.0/gtk.css");
    
    //app_set_theme("./themes/equilux-theme-v20170913/Equilux/gtk-3.22/gtk.css");


    // GdkScrollDirection direction; // GDK_SCROLL_DOWN, GDK_SCROLL_SMOOTH, GDK_SCROLL_LEFT, GDK_SCROLL_RIGHT
    //g_signal_connect(G_OBJECT(renderingWindow), "keyboard_press", G_CALLBACK(key_press_cb), nullptr);
    //gtk_window_set_focus_on_map(renderingWindow, true);

    g_signal_emit_by_name(G_OBJECT(drawingArea), "activate");
    gtk_widget_grab_focus(drawingArea);
}

static void startup(GApplication *app){

    GResource *resource = gtkofd_get_resource();
    if (resource == nullptr){
        LOG_ERROR("%s", "gtkofd_get_resource() return nullptr.");
    }
    g_resources_register(resource);
    m_readWindow->resource = resource;

    //GtkBuilder *builder = gtk_builder_new_from_file("./app.ui");
    //GtkBuilder *builder = gtk_builder_new_from_resource("/ui/app.ui");
    //GtkBuilder *builder = gtk_builder_new_from_file("./menus.ui");
    GtkBuilder *builder = gtk_builder_new_from_resource("/ui/menus.ui");

    //GObject *appMenu = gtk_builder_get_object(builder, "appmenu");
    GObject *menuBar = gtk_builder_get_object(builder, "menubar");
    //GObject *menuBar = gtk_builder_get_object(builder, "mainMenuBar");
    //gtk_application_set_app_menu(GTK_APPLICATION(app), G_MENU_MODEL(appMenu));
    gtk_application_set_menubar(GTK_APPLICATION(app), G_MENU_MODEL(menuBar));

    g_object_unref (builder);


    std::string filename = "./data/1.ofd";
    ofd::DocumentPtr document = m_readWindow->OpenOFDFile(filename);
    if (document != nullptr){
        size_t total_pages = document->GetNumPages();
        LOG_INFO("%d pages in %s", total_pages, filename.c_str());
        //if ( total_pages > 0 ){
            //int screenWidth = 794;
            //int screenHeight = 1122;
            //int screenBPP = 32;
        //}
    }

}

static gint command_line(GApplication *app, GApplicationCommandLine *cmdline){

    activate(app);

    return 0;
}

__attribute__((unused)) static void show_action_dialog(GSimpleAction *action){
    const gchar *name;
    GtkWidget *dialog;

    name = g_action_get_name(G_ACTION(action));
    dialog = gtk_message_dialog_new(nullptr,
            GTK_DIALOG_DESTROY_WITH_PARENT,
            GTK_MESSAGE_INFO,
            GTK_BUTTONS_CLOSE,
            "You activated action: \"%s\"",
            name);
    g_signal_connect(dialog, "response", G_CALLBACK(gtk_widget_destroy), nullptr);

    gtk_widget_show(dialog);
}

static void activate_toggle(GSimpleAction *action, GVariant *parameter, gpointer user_data){
    GVariant *state;

    //show_action_dialog(action);

    state = g_action_get_state(G_ACTION(action));
    g_action_change_state(G_ACTION(action), g_variant_new_boolean(!g_variant_get_boolean(state)));

    g_variant_unref(state);
}

static void change_theme_state(GSimpleAction *action, GVariant *state, gpointer user_data){
    GtkSettings *settings = gtk_settings_get_default();

    g_object_set(G_OBJECT(settings),
            "gtk-application-prefer-dark-theme",
            g_variant_get_boolean(state),
            nullptr);

    g_simple_action_set_state(action, state);
}

int main(int argc, char *argv[]){

    utils::Logger::Initialize(1);

    m_readWindow = std::make_shared<ReadWindow>();

    GtkApplication *app;
    static GActionEntry app_entries[] = {
        { "about", activate_about, nullptr, nullptr, nullptr },
        { "quit", activate_quit, nullptr, nullptr, nullptr },
        { "dark", activate_toggle, nullptr, "false", change_theme_state },
    };

    app = gtk_application_new("org.ofd.Reader", 
            //GApplicationFlags(G_APPLICATION_FLAGS_NONE));
            GApplicationFlags(G_APPLICATION_NON_UNIQUE|G_APPLICATION_HANDLES_COMMAND_LINE));

    g_action_map_add_action_entries(G_ACTION_MAP(app),
            app_entries, G_N_ELEMENTS(app_entries),
            app);
    
    g_signal_connect(app, "startup", G_CALLBACK(startup), NULL);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    g_signal_connect(app, "command-line", G_CALLBACK(command_line), NULL);
    //g_signal_connect(app, "handle-local-options", G_CALLBACK(local_options), NULL);

    g_application_run(G_APPLICATION(app), argc, argv);

    return 0;
}
