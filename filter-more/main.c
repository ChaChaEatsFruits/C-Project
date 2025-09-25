#include <gtk/gtk.h>
#include "helpers.h"
#include <stdlib.h>
#include <gdk-pixbuf-2.0/gdk-pixbuf/gdk-pixbuf-core.h>

// A struct to hold pointers to widgets we need to access in different functions
typedef struct
{
    GtkWidget *window;
    GtkWidget *image_display;
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

    // Allocate memory for the 2D array
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
    // Create a new pixbuf
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
            guchar *p = pixels + i * rowstride + j * 3; // 3 channels (RGB)
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
        return; // No image loaded
    }

    int height, width;
    // Convert pixbuf to our custom format
    RGBTRIPLE (*image_data)[width] = pixbuf_to_rgbtriple(widgets->current_pixbuf, &height, &width);
    if (!image_data)
    {
        g_print("Failed to convert pixbuf to RGBTRIPLE array.\n");
        return;
    }

    // Apply the selected filter
    if (g_strcmp0(filter_name, "Grayscale") == 0)      grayscale(height, width, image_data);
    else if (g_strcmp0(filter_name, "Reflect") == 0)   reflect(height, width, image_data);
    else if (g_strcmp0(filter_name, "Blur") == 0)      blur(height, width, image_data);
    else if (g_strcmp0(filter_name, "Edges") == 0)     edges(height, width, image_data);
    else if (g_strcmp0(filter_name, "Sepia") == 0)     sepia(height, width, image_data);
    else if (g_strcmp0(filter_name, "Negative") == 0)  negative(height, width, image_data);
    else if (g_strcmp0(filter_name, "Sharpen") == 0)   sharpen(height, width, image_data);
    else if (g_strcmp0(filter_name, "Emboss") == 0)    emboss(height, width, image_data);

    // Convert back to pixbuf
    GdkPixbuf *new_pixbuf = rgbtriple_to_pixbuf(height, width, image_data);
    free(image_data); // Free the memory for our custom array

    if (new_pixbuf)
    {
        // Free the old pixbuf and update the UI
        g_object_unref(widgets->current_pixbuf);
        widgets->current_pixbuf = new_pixbuf;
        gtk_image_set_from_pixbuf(GTK_IMAGE(widgets->image_display), widgets->current_pixbuf);
    }
}


// Callback for the "Open" button
static void on_open_clicked(GtkButton *button, gpointer user_data)
{
    AppWidgets *widgets = (AppWidgets *)user_data;
    GtkWidget *dialog = gtk_file_chooser_dialog_new("Open Image", GTK_WINDOW(widgets->window),
                                                    GTK_FILE_CHOOSER_ACTION_OPEN,
                                                    "_Cancel", GTK_RESPONSE_CANCEL,
                                                    "_Open", GTK_RESPONSE_ACCEPT,
                                                    NULL);

    // Add image file filters
    GtkFileFilter *filter = gtk_file_filter_new();
    gtk_file_filter_set_name(filter, "Image Files");
    gtk_file_filter_add_pixbuf_formats(filter);
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT)
    {
        GFile *file = gtk_file_chooser_get_file(GTK_FILE_CHOOSER(dialog));
        if (file)
        {
            if (widgets->current_pixbuf)
            {
                g_object_unref(widgets->current_pixbuf);
            }
            widgets->current_pixbuf = gdk_pixbuf_new_from_gfile(file, NULL, NULL);
            if (widgets->current_pixbuf)
            {
                gtk_image_set_from_pixbuf(GTK_IMAGE(widgets->image_display), widgets->current_pixbuf);
                // Enable filter and save buttons
                gtk_widget_set_sensitive(widgets->filter_box, TRUE);
                gtk_widget_set_sensitive(widgets->save_button, TRUE);
            }
            g_object_unref(file);
        }
    }
    gtk_widget_destroy(dialog);
}

// Callback for the "Save" button
static void on_save_clicked(GtkButton *button, gpointer user_data)
{
    AppWidgets *widgets = (AppWidgets *)user_data;
     GtkWidget *dialog = gtk_file_chooser_dialog_new("Save Image", GTK_WINDOW(widgets->window),
                                                    GTK_FILE_CHOOSER_ACTION_SAVE,
                                                    "_Cancel", GTK_RESPONSE_CANCEL,
                                                    "_Save", GTK_RESPONSE_ACCEPT,
                                                    NULL);

    gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(dialog), "filtered-image.png");

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT)
    {
        GFile *file = gtk_file_chooser_get_file(GTK_FILE_CHOOSER(dialog));
        if (file)
        {
            const char *path = g_file_get_path(file);
            gdk_pixbuf_save(widgets->current_pixbuf, path, "png", NULL, NULL);
            g_object_unref(file);
        }
    }
    gtk_widget_destroy(dialog);
}


// This function is called when the application is activated
static void activate(GtkApplication *app, gpointer user_data)
{
    AppWidgets *widgets = (AppWidgets *)user_data;

    // Main Window
    widgets->window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(widgets->window), "C Image Filters");
    gtk_window_set_default_size(GTK_WINDOW(widgets->window), 800, 600);

    // Header Bar
    GtkWidget *header = gtk_header_bar_new();
    gtk_header_bar_set_show_title_buttons(GTK_HEADER_BAR(header), TRUE);
    gtk_window_set_titlebar(GTK_WINDOW(widgets->window), header);

    // Open and Save Buttons in Header
    GtkWidget *open_button = gtk_button_new_with_label("Open");
    g_signal_connect(open_button, "clicked", G_CALLBACK(on_open_clicked), widgets);
    gtk_header_bar_pack_start(GTK_HEADER_BAR(header), open_button);

    widgets->save_button = gtk_button_new_with_label("Save");
    g_signal_connect(widgets->save_button, "clicked", G_CALLBACK(on_save_clicked), widgets);
    gtk_header_bar_pack_end(GTK_HEADER_BAR(header), widgets->save_button);
    gtk_widget_set_sensitive(widgets->save_button, FALSE); // Initially disabled

    // Main Vertical Box
    GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_window_set_child(GTK_WINDOW(widgets->window), main_box);

    // Image Display Area
    widgets->image_display = gtk_image_new();
    gtk_image_set_icon_size(GTK_IMAGE(widgets->image_display), GTK_ICON_SIZE_LARGE);
    gtk_widget_set_vexpand(widgets->image_display, TRUE);
    gtk_box_append(GTK_BOX(main_box), widgets->image_display);

    // Box for Filter Buttons
    widgets->filter_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_widget_set_halign(widgets->filter_box, GTK_ALIGN_CENTER);
    gtk_box_append(GTK_BOX(main_box), widgets->filter_box);

    // Create Filter Buttons
    const char *filter_names[] = {"Grayscale", "Reflect", "Blur", "Edges", "Sepia", "Negative", "Sharpen", "Emboss"};
    for (int i = 0; i < G_N_ELEMENTS(filter_names); i++)
    {
        GtkWidget *button = gtk_button_new_with_label(filter_names[i]);
        g_signal_connect(button, "clicked", G_CALLBACK(apply_filter), widgets);
        gtk_box_append(GTK_BOX(widgets->filter_box), button);
    }
    gtk_widget_set_sensitive(widgets->filter_box, FALSE); // Initially disabled

    gtk_window_present(GTK_WINDOW(widgets->window));
}

int main(int argc, char **argv)
{
    GtkApplication *app;
    int status;

    // Allocate our main widget struct
    AppWidgets *widgets = g_new(AppWidgets, 1);
    widgets->current_pixbuf = NULL;

    app = gtk_application_new("com.example.cimagefilters", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), widgets);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    // Clean up
    if (widgets->current_pixbuf)
    {
        g_object_unref(widgets->current_pixbuf);
    }
    g_free(widgets);

    return status;
}