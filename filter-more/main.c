#include <gtk/gtk.h>
#include "helpers.h"
#include <stdlib.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

// A struct to hold pointers to widgets we need to access in different functions
typedef struct
{
    GtkWindow *window;
    GtkImage *image_display;
    GtkWidget *filter_box;
    GtkWidget *save_button;
    GdkPixbuf *current_pixbuf;
} AppWidgets;

// Function to convert GdkPixbuf to a 2D RGBTRIPLE array
RGBTRIPLE (*pixbuf_to_rgbtriple(GdkPixbuf *pixbuf, int *height, int *width))[0]
{
    *width = gdk_pixbuf_get_width(pixbuf);
    *height = gdk_pixbuf_get_height(pixbuf);
    int n_channels = gdk_pixbuf_get_n_channels(pixbuf);
    int rowstride = gdk_pixbuf_get_rowstride(pixbuf);
    guchar *pixels = gdk_pixbuf_get_pixels(pixbuf);

    RGBTRIPLE (*image)[*width] = malloc(sizeof(RGBTRIPLE[*height][*width]));
    if (!image)
    {
        return NULL;
    }

    for (int i = 0; i < *height; i++)
    {
        for (int j = 0; j < *width; j++)
        {
            guchar *p = pixels + i * rowstride + j * n_channels;
            image[i][j].rgbtRed = p[0];
            image[i][j].rgbtGreen = p[1];
            image[i][j].rgbtBlue = p[2];
        }
    }
    return image;
}

// Function to convert a 2D RGBTRIPLE array back to a GdkPixbuf
GdkPixbuf *rgbtriple_to_pixbuf(int height, int width, RGBTRIPLE image[height][width])
{
    GdkPixbuf *pixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, FALSE, 8, width, height);
    if (!pixbuf)
    {
        return NULL;
    }

    int rowstride = gdk_pixbuf_get_rowstride(pixbuf);
    guchar *pixels = gdk_pixbuf_get_pixels(pixbuf);

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            guchar *p = pixels + i * rowstride + j * 3;
            p[0] = image[i][j].rgbtRed;
            p[1] = image[i][j].rgbtGreen;
            p[2] = image[i][j].rgbtBlue;
        }
    }
    return pixbuf;
}

// Generic function to apply a filter
static void apply_filter(GtkButton *button, gpointer user_data)
{
    AppWidgets *widgets = (AppWidgets *)user_data;
    const char *filter_name = gtk_button_get_label(button);

    if (!widgets->current_pixbuf)
    {
        return;
    }

    int height, width;
    RGBTRIPLE (*image_data)[width] = pixbuf_to_rgbtriple(widgets->current_pixbuf, &height, &width);
    if (!image_data)
    {
        g_print("Failed to convert pixbuf to RGBTRIPLE array.\n");
        return;
    }

    if (g_strcmp0(filter_name, "Grayscale") == 0)      grayscale(height, width, image_data);
    else if (g_strcmp0(filter_name, "Reflect") == 0)   reflect(height, width, image_data);
    else if (g_strcmp0(filter_name, "Blur") == 0)      blur(height, width, image_data);
    else if (g_strcmp0(filter_name, "Edges") == 0)     edges(height, width, image_data);
    else if (g_strcmp0(filter_name, "Sepia") == 0)     sepia(height, width, image_data);
    else if (g_strcmp0(filter_name, "Negative") == 0)  negative(height, width, image_data);
    else if (g_strcmp0(filter_name, "Sharpen") == 0)   sharpen(height, width, image_data);
    else if (g_strcmp0(filter_name, "Emboss") == 0)    emboss(height, width, image_data);

    GdkPixbuf *new_pixbuf = rgbtriple_to_pixbuf(height, width, image_data);
    free(image_data);

    if (new_pixbuf)
    {
        g_object_unref(widgets->current_pixbuf);
        widgets->current_pixbuf = new_pixbuf;
        gtk_image_set_from_paintable(widgets->image_display, GDK_PAINTABLE(widgets->current_pixbuf));
    }
}

// Modern GTK4 callback for the "Open" dialog
static void open_cb(GObject *source_object, GAsyncResult *res, gpointer user_data)
{
    AppWidgets *widgets = (AppWidgets *)user_data;
    GtkFileDialog *dialog = GTK_FILE_DIALOG(source_object);
    GFile *file = gtk_file_dialog_open_finish(dialog, res, NULL);

    if (file)
    {
        if (widgets->current_pixbuf)
        {
            g_object_unref(widgets->current_pixbuf);
        }
        char *path = g_file_get_path(file);
        // Corrected function call with only two arguments
        widgets->current_pixbuf = gdk_pixbuf_new_from_file(path, NULL);
        g_free(path);

        if (widgets->current_pixbuf)
        {
            gtk_image_set_from_paintable(widgets->image_display, GDK_PAINTABLE(widgets->current_pixbuf));
            gtk_widget_set_sensitive(widgets->filter_box, TRUE);
            gtk_widget_set_sensitive(widgets->save_button, TRUE);
        }
        g_object_unref(file);
    }
}

// Callback for the "Open" button, using modern GTK4 API
static void on_open_clicked(GtkButton *button, gpointer user_data)
{
    AppWidgets *widgets = (AppWidgets *)user_data;
    GtkFileDialog *dialog = gtk_file_dialog_new();
    gtk_file_dialog_open(dialog, widgets->window, NULL, open_cb, user_data);
    g_object_unref(dialog);
}

// Modern GTK4 callback for the "Save" dialog
static void save_cb(GObject *source_object, GAsyncResult *res, gpointer user_data)
{
    AppWidgets *widgets = (AppWidgets *)user_data;
    GtkFileDialog *dialog = GTK_FILE_DIALOG(source_object);
    GFile *file = gtk_file_dialog_save_finish(dialog, res, NULL);

    if (file)
    {
        char *path = g_file_get_path(file);
        gdk_pixbuf_save(widgets->current_pixbuf, path, "png", NULL, NULL);
        g_free(path);
        g_object_unref(file);
    }
}

// Callback for the "Save" button, using modern GTK4 API
static void on_save_clicked(GtkButton *button, gpointer user_data)
{
    AppWidgets *widgets = (AppWidgets *)user_data;
    GtkFileDialog *dialog = gtk_file_dialog_new();
    gtk_file_dialog_set_initial_name(dialog, "filtered-image.png");
    gtk_file_dialog_save(dialog, widgets->window, NULL, save_cb, user_data);
    g_object_unref(dialog);
}

// This function is called when the application is activated
static void activate(GtkApplication *app, gpointer user_data)
{
    AppWidgets *widgets = (AppWidgets *)user_data;

    widgets->window = GTK_WINDOW(gtk_application_window_new(app));
    gtk_window_set_title(widgets->window, "C Image Filters");
    gtk_window_set_default_size(widgets->window, 800, 600);

    GtkWidget *header = gtk_header_bar_new();
    gtk_window_set_titlebar(widgets->window, header);

    GtkWidget *open_button = gtk_button_new_with_label("Open");
    g_signal_connect(open_button, "clicked", G_CALLBACK(on_open_clicked), widgets);
    gtk_header_bar_pack_start(GTK_HEADER_BAR(header), open_button);

    widgets->save_button = gtk_button_new_with_label("Save");
    g_signal_connect(widgets->save_button, "clicked", G_CALLBACK(on_save_clicked), widgets);
    gtk_header_bar_pack_end(GTK_HEADER_BAR(header), widgets->save_button);
    gtk_widget_set_sensitive(widgets->save_button, FALSE);

    GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_window_set_child(widgets->window, main_box);

    widgets->image_display = GTK_IMAGE(gtk_image_new());
    gtk_widget_set_vexpand(GTK_WIDGET(widgets->image_display), TRUE);
    gtk_box_append(GTK_BOX(main_box), GTK_WIDGET(widgets->image_display));

    widgets->filter_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_widget_set_halign(widgets->filter_box, GTK_ALIGN_CENTER);
    gtk_box_append(GTK_BOX(main_box), widgets->filter_box);

    const char *filter_names[] = {"Grayscale", "Reflect", "Blur", "Edges", "Sepia", "Negative", "Sharpen", "Emboss"};
    for (int i = 0; i < G_N_ELEMENTS(filter_names); i++)
    {
        GtkWidget *button = gtk_button_new_with_label(filter_names[i]);
        g_signal_connect(button, "clicked", G_CALLBACK(apply_filter), widgets);
        gtk_box_append(GTK_BOX(widgets->filter_box), button);
    }
    gtk_widget_set_sensitive(widgets->filter_box, FALSE);

    gtk_window_present(widgets->window);
}

int main(int argc, char **argv)
{
    GtkApplication *app;
    int status;

    AppWidgets *widgets = g_new(AppWidgets, 1);
    widgets->current_pixbuf = NULL;

    app = gtk_application_new("com.example.cimagefilters", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), widgets);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    if (widgets->current_pixbuf)
    {
        g_object_unref(widgets->current_pixbuf);
    }
    g_free(widgets);

    return status;
}

