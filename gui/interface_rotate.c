#include "interface_rotate.h"

#include "../utils/rotateutils.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>

static double rotate = 0;

typedef struct gtk_data Gtk_Data;
struct gtk_data
{
  GtkWidget *widget;
  char *path;
  char *orig_path;
  double *angle;
};

void
rotate_left(GtkWidget *widget, gpointer *data)
{
  UNUSED(widget);
  SDL_Surface *new_image;
  SDL_Surface *image = load_image(((Gtk_Data *) data)->orig_path);
  rotate -= ANGLE;
  new_image = rotate_image(image, rotate);
  IMG_SavePNG(new_image, ((Gtk_Data *) data)->path);
  SDL_FreeSurface(image);
  SDL_FreeSurface(new_image);
  gtk_image_clear((GtkImage *) ((Gtk_Data *) data)->widget);
  gtk_image_set_from_file((GtkImage *) ((Gtk_Data *) data)->widget,
                          ((Gtk_Data *) data)->path);
}

void
rotate_right(GtkWidget *widget, gpointer *data)
{
  UNUSED(widget);
  SDL_Surface *image = load_image(((Gtk_Data *) data)->orig_path);
  rotate += ANGLE;
  SDL_Surface *new_image = rotate_image(image, rotate);
  IMG_SavePNG(new_image, ((Gtk_Data *) data)->path);
  SDL_FreeSurface(image);
  SDL_FreeSurface(new_image);
  gtk_image_clear((GtkImage *) ((Gtk_Data *) data)->widget);
  gtk_image_set_from_file((GtkImage *) ((Gtk_Data *) data)->widget,
                          ((Gtk_Data *) data)->path);
}

void
destroy(GtkWidget *widget, gpointer data)
{
  UNUSED(widget);
  UNUSED(data);
  gtk_main_quit();
}

int
main(int argc, char **argv)
{
  GtkWidget *window;
  GtkWidget *button_left;
  GtkWidget *button_right;
  GtkWidget *image;
  GtkWidget *box;
  char name[] = "/tmp/fileXXXXXX";
  int fd = mkstemp(name);
  printf("%s\n", name);
  printf("%p\n", name);
  close(fd);
  char *temp = calloc(16, 1);
  snprintf(temp, 16, "%s", name);
  printf("%p\n", temp);
  printf("%s\n", temp);
  Gtk_Data data_to_pass;

  if (argc != 2) {
    printf("Usage: %s path_to_image\n", argv[0]);
    return 1;
  }

  gtk_init(&argc, &argv);

  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  box = gtk_box_new(FALSE, 0);
  g_signal_connect(window, "destroy", G_CALLBACK(destroy), NULL);

  button_left = gtk_button_new_with_label("<Left");
  button_right = gtk_button_new_with_label("Right>");
  image = gtk_image_new_from_file(argv[1]);

  if (!temp) {
    printf("ALED\n");
    return 1;
  }

  SDL_Surface *image_temp = load_image(argv[1]);
  IMG_SavePNG(image_temp, temp);
  SDL_FreeSurface(image_temp);

  data_to_pass.widget = image;
  data_to_pass.path = temp;
  data_to_pass.orig_path = argv[1];

  g_signal_connect(button_left, "clicked", G_CALLBACK(rotate_left),
                   (gpointer) &data_to_pass);
  g_signal_connect(button_right, "clicked", G_CALLBACK(rotate_right),
                   (gpointer) &data_to_pass);

  gtk_container_add((GtkContainer *) box, button_left);
  gtk_container_add((GtkContainer *) box, button_right);
  gtk_container_add((GtkContainer *) box, image);

  SDL_Surface *new_image;
  printf("%s\n", data_to_pass.path);
  SDL_Surface *k_image = load_image(data_to_pass.orig_path);
  new_image = rotate_image(k_image, rotate);
  IMG_SavePNG(new_image, data_to_pass.path);
  SDL_FreeSurface(k_image);
  gtk_image_clear((GtkImage *) data_to_pass.widget);
  gtk_image_set_from_file((GtkImage *) data_to_pass.widget, data_to_pass.path);
  SDL_FreeSurface(new_image);

  gtk_container_add(GTK_CONTAINER(window), box);

  gtk_widget_show_all(window);
  gtk_main();

  free(temp);

  return 0;
}
