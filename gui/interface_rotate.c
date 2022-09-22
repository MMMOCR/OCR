#include <gtk/gtk.h>
#include <gdk/gdkx.h>
#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>


static SDL_Window *sdl_window;
static SDL_Renderer *sdl_renderer;
static void *window_id;

int
idle (void *ud)
{
  if(!sdl_window) {
    printf("creating SDL window for window id %p\n", window_id);
    sdl_window = SDL_CreateWindowFrom(window_id);
    if (sdl_window == NULL) {
      printf("error");
      return 0;
    }
    printf("sdl_window=%p\n", (void *)sdl_window);
    if(!sdl_window) {
      printf("%s\n", SDL_GetError());
    }
    sdl_renderer = SDL_CreateRenderer(sdl_window, -1, 0);
    printf("sdl_renderer=%p\n", (void *)sdl_renderer);
    if(!sdl_renderer) {
      printf("%s\n", SDL_GetError());
    }
  } else {
    SDL_SetRenderDrawColor(sdl_renderer, 255, 0, 0, 255);
    SDL_RenderClear(sdl_renderer);
    SDL_RenderPresent(sdl_renderer);
  }

  return 1;
}


int
main ( int argc, char **argv)
{
    GtkWindow *gtk_window;
    GtkWidget *gtk_da;
    void *gdk_window;


    if (argc != 2){
      printf("Usage: %s path_to_image\n", argv[0]);
      return 1;
    }

    gtk_init (&argc , &argv);
 
    //init window and set title
    gtk_window = (GtkWindow*)gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(gtk_window, "pikalul");

    // connect close window
    g_signal_connect(gtk_window,  "delete-event", gtk_main_quit, NULL);

    // create drawing area and show
    gtk_da = gtk_drawing_area_new();
    gtk_container_add(GTK_CONTAINER(gtk_window), gtk_da);
    gtk_widget_show_all(GTK_WIDGET(gtk_window));

    // get window socket id
    gdk_window = gtk_widget_get_window(GTK_WIDGET(gtk_da));
    window_id = (void*)(intptr_t)GDK_WINDOW_XID(gdk_window);

    printf("pointer window id: %p\n", window_id);

    // init sdl and connect to gtk

    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO) != 0) {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return 1;
    }

    g_idle_add(&idle, 0);

    gtk_main();

    return 0;
}
