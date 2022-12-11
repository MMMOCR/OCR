//
// Created by rigole on 10/10/22.
//

#include "impl/SDLRenderer/sypbc_sdlrenderer.h"
#include "utils/animations/linear.h"
#include "widgets/button.h"

#include <gtk/gtk.h>
#include <libgen.h>
#include <pthread.h>

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720
#define STEPS_COUNT 6

enum gui_states
{
    Load,
    Main,
    Error
};

enum steps
{
    Binarization,
    Grid,
    Rotation,
    Numbers,
    Solving,
    Output
};

static char *steps[STEPS_COUNT] = { "Binarization", "Grid detection",
                                    "Rotation",     "Numbers detection",
                                    "Solving",      "Output" };
static char *descs[STEPS_COUNT] = {
    "Binarizes the image so that detecting the grid and numbers becomes easier",
    "Detects the grid and crops the image",
    "Rotates automatically the image",
    "Detects numbers using a neural network",
    "Solves the sudoku using the backtracking algorithm",
    "Output of the OCR sudoku solver that you can save (Tips: hold left button "
    "on the image to show the input)"
};
static int current_step = 0;
static int selected_step = 0;
static char working = 0;
static char finished = 0;

static enum gui_states current_gui_state = Main;
static pthread_t task_thread;

static int circle_step = 0;
static linear_anim steps_anim = { 255, 1.2, 0, 1, 0 };
static linear_anim currentx_anim = { 255, 0.2, 0, 1, 0 };
static linear_anim currenty_anim = { 255, 0.2, 0, 1, 0 };

/* Error */
static struct sypbc_button back_button;

/* Load widgets */
static struct sypbc_button load_load_button;
static struct sypbc_button load_weights_button;
static struct sypbc_button website_button;
static linear_anim load_hide = { WINDOW_HEIGHT, 0.1, 0, 0, 0 };

/* Main widgets */
static struct sypbc_button load_button;
static struct sypbc_button save_button;
static struct sypbc_button select_button;
static struct sypbc_button rotate_left;
static struct sypbc_button rotate_right;
static struct sypbc_button validate_rotation;

static char *images[] = {
    "steps/resized.png", "steps/grid_detection.png",
    "steps/rotation.png",     "steps/rotation.png",
    "steps/rotation.png",      "steps/output.png"
};

static char *weigths_path;
static char *image_path = "/home/rigole/Documents/FinalOCR/test1.jpg";

enum dialog_type
{
    FILE_OPEN,
    FILE_OPEN_DIR,
    FILE_SAVE,
};

char *
dialog_file(enum dialog_type action, const char *dir, const char *filename)
{
    if (!gtk_init_check(NULL, NULL)) return 0;

    GtkFileChooserAction gtkAction;
    const char *title;
    const char *acceptText;
    if (action == FILE_OPEN) {
        title = "Open File";
        acceptText = "Open";
        gtkAction = GTK_FILE_CHOOSER_ACTION_OPEN;
    } else if (action == FILE_OPEN_DIR) {
        title = "Open Folder";
        acceptText = "Open Folder";
        gtkAction = GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER;
    } else {
        title = "Save File";
        acceptText = "Save";
        gtkAction = GTK_FILE_CHOOSER_ACTION_SAVE;
    }

    GtkWidget *dialog = gtk_file_chooser_dialog_new(
      title, NULL, gtkAction, "_Cancel", GTK_RESPONSE_CANCEL, acceptText,
      GTK_RESPONSE_ACCEPT, NULL);

    if (action == FILE_SAVE)
        gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(dialog),
                                                       TRUE);

    if (dir) gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialog), dir);

    if (action == FILE_SAVE && filename)
        gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(dialog), filename);

    char *chosen_filename = NULL;
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        chosen_filename =
          gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
    }
    gtk_widget_destroy(dialog);

    char *result = NULL;
    if (chosen_filename) {
        result = strndup(chosen_filename, strlen(chosen_filename));
        g_free(chosen_filename);
    }

    while (gtk_events_pending())
        gtk_main_iteration();
    return result;
}

void
reset_gui()
{
    working = 0;
    finished = 0;
    current_step = 0;
    selected_step = 0;
    circle_step = 0;
    select_button.enabled = 0;
    save_button.enabled = 0;
    save_button.text_color = color_rgb(121, 121, 121);
    currentx_anim.current = 0;
    currenty_anim.current = 0;
    task_thread = 0;
}

void
execute_task(struct sypbc *ctx)
{
    char pim[512];
    strcpy(pim, ctx->images_path);
    strcat(pim, images[selected_step]);

    switch (selected_step) {
        case 0: {
            char pbin[512];
            strcpy(pbin, ctx->bin_path);
            strcat(pbin, "imageutils");
            strcat(pbin, " ");
            strcat(pbin, image_path);

            system(pbin);
            break;
        }
        case 1: {
            char pbin[512];
            strcpy(pbin, ctx->bin_path);
            strcat(pbin, "linesdetection");
            strcat(pbin, " ");
            strcat(pbin, ctx->images_path);
            strcat(pbin, "steps/binarization.png");
            strcat(pbin, " ");
            strcat(pbin, ctx->images_path);
            strcat(pbin, "steps/grayscale.png");

            printf("%s", pbin);
            system(pbin);
            break;
        }
    }

    if (access(pim, F_OK) != 0) {
        //current_gui_state = Error;
        //reset_gui();
    } else if (!working)
        working = 1;
    else if (current_step != Grid) {
        current_step++;
    }

    task_thread = 0;
}

void
draw_main(struct sypbc *ctx)
{
    /* Navbar */
    {
        draw_push_filled_rect(&ctx->draw, 0, 0, WINDOW_WIDTH, 83,
                              color_rgb(27, 27, 27)); // Background
        draw_push_text(&ctx->draw, "OCR", 30, 18, ROBOTO_BOLD45,
                       color_rgb(255, 255, 255));
        draw_push_line(&ctx->draw, 140, 28, 140, 60, 1,
                       color_rgb(184, 184, 184));
        draw_push_text(&ctx->draw, "Sudoku solver", 160, 26, ROBOTO_REGULAR32,
                       color_rgb(184, 184, 184));
        if (sypbc_image_button_draw(&load_button, "add.png", 20)) {
            char *new_image_path;
            if ((new_image_path = dialog_file(FILE_OPEN, 0, 0))) {
                image_path = new_image_path;
                reset_gui();
            }
        }
    }

    if (finished && is_cursor_within(ctx->input, 494, 135, 660, 430) &&
        is_mouse_pressed(ctx->input, 1)) {
        draw_push_raw_image(&ctx->draw, image_path, 494, 135);
    } else {
        if (working) {
            draw_push_image(&ctx->draw,
                            images[(!finished ? current_step : selected_step)],
                            494, 135);
        } else {
            draw_push_image(&ctx->draw, image_path, 494, 135);
            if (!task_thread && current_step == 0) {
                pthread_create(&task_thread, NULL,
                               (void *(*) (void *) ) execute_task, ctx);
            }
        }
    }

    if (finished) {
        draw_push_filled_rounded_rect(
          &ctx->draw, 372, 630, 875, 65, 10,
          color_rgb(27, 27, 27)); // Description background

        vec2 dims = get_text_dim(descs[selected_step], ROBOTO_REGULAR16);
        draw_push_text(&ctx->draw, descs[selected_step], 810 - dims.x / 2, 655,
                       ROBOTO_REGULAR16, color_rgb(184, 184, 184));
    }

    if (current_step == Grid) {
        if (sypbc_button_draw(&rotate_left)) {}

        if (sypbc_button_draw(&rotate_right)) {}

        if (sypbc_button_draw(&validate_rotation)) { current_step++; }
    }

    /* Steps */
    {
        draw_push_filled_rounded_rect(
          &ctx->draw, 30, 105, 315, 590, 10,
          color_rgb(27, 27, 27)); // Steps background

        if (sypbc_image_button_draw(&save_button,
                                    save_button.enabled ? "save.png"
                                                        : "save-disabled.png",
                                    32)) { // save image button
            char *output;
            if ((output = dialog_file(FILE_SAVE, 0, 0))) {
                char cmd[512];
                char pim[512];
                strcpy(pim, ctx->images_path);
                strcat(pim, images[STEPS_COUNT - 1]);

                sprintf(cmd, "cp %s %s", pim, output);
                system(cmd);
            }
        }
    }

    /* Loading circles */
    vec2 dims = get_text_dim(steps[(!finished ? current_step : selected_step)],
                             ROBOTO_REGULAR16);

    /* Steps animation */
    {
        currentx_anim.target = dims.x;
        currenty_anim.target =
          140 + (!finished ? current_step : selected_step) * 75 - 5;
        if (!currenty_anim.current)
            currenty_anim.current = 140 + current_step * 75 - 5;
        if (!currentx_anim.current) currentx_anim.current = dims.x;

        if (working) {
            linear_run(&currentx_anim);
            linear_run(&currenty_anim);

            if (circle_step <= 25) {
                if (selected_step == current_step) { linear_run(&steps_anim); }
                if (linear_has_finished(&steps_anim)) {
                    if (selected_step == current_step) {
                        circle_step++;
                        steps_anim.current = 0;
                    }
                    if (!task_thread && circle_step % 5 == 0) {
                        selected_step++;
                        pthread_create(&task_thread, NULL,
                                       (void *(*) (void *) ) execute_task, ctx);
                    }
                }
            } else if (!finished) {
                save_button.enabled = 1;
                save_button.text_color = color_rgb(255, 255, 255);
                selected_step = current_step;
                finished = 1;
            }

            draw_push_filled_rounded_rect(
              &ctx->draw, 90, currenty_anim.current, currentx_anim.current + 20,
              dims.y + 10, 5, color_rgb(71, 123, 203));
        }
    }

    /* Steps draw */
    {
        for (size_t i = 0; i < STEPS_COUNT; i++) {
            vec2 cdims = get_text_dim(steps[i], ROBOTO_REGULAR16);
            if (finished && (!finished ? current_step : selected_step) != i &&
                is_cursor_within(ctx->input, 90, 140 + i * 75 - 5, cdims.x + 20,
                                 cdims.y + 10)) {
                select_button.enabled = 1;
                select_button.x = 90;
                select_button.y = 140 + i * 75 - 5;
                select_button.width = cdims.x + 20;
                select_button.height = dims.y + 10;

                if (sypbc_button_draw(&select_button)) { selected_step = i; }
            }

            draw_push_filled_circle(&ctx->draw, 65, 150 + i * 75, 9,
                                    circle_step > i * 5
                                      ? color_rgb(71, 123, 203)
                                      : color_rgb(184, 184, 184));
            if (circle_step == i * 5)
                draw_push_filled_circle(
                  &ctx->draw, 65, 150 + i * 75, 9,
                  color_rgba(71, 123, 203, steps_anim.current));

            draw_push_text(&ctx->draw, steps[i], 100, 140 + i * 75,
                           ROBOTO_REGULAR16, color_rgb(255, 255, 255));

            if (i < STEPS_COUNT - 1)
                for (size_t j = 0; j < 4; j++) {
                    draw_push_filled_circle(
                      &ctx->draw, 65, 170 + i * 75 + j * 12, 2,
                      circle_step > 1 + i * 5 + j ? color_rgb(71, 123, 203)
                                                  : color_rgb(184, 184, 184));
                    if (circle_step == 1 + i * 5 + j)
                        draw_push_filled_circle(
                          &ctx->draw, 65, 170 + i * 75 + j * 12, 2,
                          color_rgba(71, 123, 203, steps_anim.current));
                }
        }
    }
}

void
draw_load(struct sypbc *ctx)
{
    linear_run(&load_hide);
    if (linear_has_finished(&load_hide)) current_gui_state = Main;

    draw_push_scissors(&ctx->draw, 0, load_hide.current, WINDOW_WIDTH,
                       WINDOW_HEIGHT);
    draw_push_filled_rect(&ctx->draw, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
                          color_rgb(27, 27, 27)); // Background
    draw_push_image(&ctx->draw, "logs.png", WINDOW_WIDTH / 2 - 90, 100);

    const char *welcome_txt = "Welcome to the OCR Sudoku solver from Epita, "
                              "please load weights and an image to continue";
    vec2 welcome_dims = get_text_dim((char *) welcome_txt, ROBOTO_REGULAR16);
    draw_push_text(&ctx->draw, (char *) welcome_txt,
                   WINDOW_WIDTH / 2 - welcome_dims.x / 2, 330, ROBOTO_REGULAR16,
                   color_rgb(121, 121, 121));
    if (sypbc_button_draw(&load_load_button)) {
        image_path = dialog_file(FILE_OPEN, 0, 0);
        if (image_path) {
            printf("Loaded image: %s\n", image_path);
            load_hide.state = 1;
        }
    }

    if (!load_load_button.enabled &&
        sypbc_button_is_within(&load_load_button)) {
        const char *warning = "Please load weights first";
        vec2 dims = get_text_dim((char *) warning, ROBOTO_REGULAR16);
        position cpos = get_cursor_pos(ctx->input);
        draw_push_filled_rounded_rect(&ctx->draw, cpos.x,
                                      cpos.y - (dims.y + 10), dims.x + 10,
                                      dims.y + 10, 5, color_rgb(32, 32, 32));
        draw_push_text(&ctx->draw, (char *) warning, cpos.x + 5,
                       cpos.y - (dims.y + 10) + 5, ROBOTO_REGULAR16,
                       color_rgb(121, 121, 121));
    }

    if (sypbc_button_draw(&load_weights_button)) {
        weigths_path = dialog_file(FILE_OPEN, 0, 0);
        if (weigths_path) {
            load_load_button.text_color = color_rgb(255, 255, 255);
            load_load_button.enabled = 1;
        }
    }

    if (sypbc_image_button_draw(&website_button, "website.png", 28)) {
        system("firefox google.com");
    }

    draw_push_scissors(&ctx->draw, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
}

void
draw_error(struct sypbc *ctx)
{
    draw_push_filled_rect(&ctx->draw, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
                          color_rgb(27, 27, 27)); // Background
    const char *error_text =
      "Your weights or image is wrong, please reselect them";
    vec2 dims = get_text_dim(error_text, ROBOTO_REGULAR16);
    draw_push_text(&ctx->draw, error_text, WINDOW_WIDTH / 2 - dims.x / 2,
                   WINDOW_HEIGHT / 2 - dims.y / 2, ROBOTO_REGULAR16,
                   color_rgb(121, 121, 121));

    if (sypbc_button_draw(&back_button)) {
        load_load_button.enabled = 0;
        load_load_button.text_color = color_rgb(180, 180, 180);
        weigths_path = 0;
        image_path = 0;
        current_gui_state = Load;
    }
}

int
main(int argc, char **argv)
{
    static SDL_Surface *screen_surface;
    struct sypbc *ctx;
    char running = 1;

    struct sypbc_font font = { 0 };

    if (SDL_Init(SDL_INIT_VIDEO) == -1) {
        printf("Can't init SDL:  %s\n", SDL_GetError());
        return 1;
    }

    screen_surface =
      SDL_SetVideoMode(WINDOW_WIDTH, WINDOW_HEIGHT, 32, SDL_SWSURFACE);
    if (screen_surface == NULL) {
        printf("Can't set video mode: %s\n", SDL_GetError());
        return 1;
    }

    SDL_WM_SetCaption("OCR Epita", NULL);

    ctx = sdl_init(screen_surface, dirname(argv[0]));

    /* Error init */
    back_button =
      sypbc_button_init(ctx, "Back to menu", ROBOTO_REGULAR16,
                        WINDOW_WIDTH / 2 - 100, WINDOW_HEIGHT / 2 + 40, 200, 50,
                        5, color_rgb(32, 32, 32), color_rgb(255, 255, 255));

    /* Load init */
    load_load_button = sypbc_button_init(
      ctx, "Load image", ROBOTO_REGULAR16, WINDOW_WIDTH / 2 - 285 / 2, 400, 285,
      50, 5, color_rgb(71, 123, 203), color_rgb(180, 180, 180));
    load_weights_button = sypbc_button_init(
      ctx, "Load weights", ROBOTO_REGULAR16, WINDOW_WIDTH / 2 - 285 / 2, 470,
      285, 50, 5, color_rgb(32, 32, 32), color_rgb(255, 255, 255));
    website_button =
      sypbc_button_init(ctx, "", ROBOTO_REGULAR16, 10, WINDOW_HEIGHT - 60, 50,
                        50, 2, color_rgb(32, 32, 32), color_rgb(255, 255, 255));
    load_load_button.enabled = 0;

    /* Main init */
    load_button =
      sypbc_button_init(ctx, "Load image", ROBOTO_REGULAR16, 1100, 18, 145, 50,
                        5, color_rgb(32, 32, 32), color_rgb(255, 255, 255));
    save_button = sypbc_button_init(ctx, "Save output image", ROBOTO_REGULAR16,
                                    45, 630, 285, 50, 5, color_rgb(32, 32, 32),
                                    color_rgb(121, 121, 121));
    select_button =
      sypbc_button_init(ctx, "", 0, 90, 0, 0, 0, 5,
                        color_rgba(255, 255, 255, 15), color_rgba(0, 0, 0, 0));
    rotate_left =
      sypbc_button_init(ctx, "-90", ROBOTO_REGULAR16, 500 + 200, 630, 50, 50, 2,
                        color_rgb(32, 32, 32), color_rgb(255, 255, 255));
    rotate_right =
      sypbc_button_init(ctx, "+90", ROBOTO_REGULAR16, 560 + 200, 630, 50, 50, 2,
                        color_rgb(32, 32, 32), color_rgb(255, 255, 255));
    validate_rotation =
      sypbc_button_init(ctx, "Done", ROBOTO_REGULAR16, 620 + 200, 630, 100, 50,
                        2, color_rgb(71, 123, 203), color_rgb(255, 255, 255));
    save_button.enabled = 0;
    select_button.enabled = 0;

    gtk_init(&argc, &argv);

    rgba bg = { 38, 38, 38, 255 };
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) goto cleanup;
            sdl_event_callback(ctx->input, &event);
        }

        switch (current_gui_state) {
            case Main: {
                draw_main(ctx);
                break;
            }
            case Load: {
                if (load_hide.state) draw_main(ctx);
                draw_load(ctx);
                break;
            }
            case Error: {
                draw_error(ctx);
                break;
            }
        }

        SDL_Delay(10);
        sdl_render(&ctx->draw, bg);
    }

cleanup:
    sdl_destroy();
    SDL_Quit();
}