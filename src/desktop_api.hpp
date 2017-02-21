#ifndef DESKTOP_API_HPP
#define DESKTOP_API_HPP

#include <libweston-1/compositor.h>
#include <libweston-1/libweston-desktop.h>

#include "commonincludes.hpp"
#include "core.hpp"

void desktop_surface_added(weston_desktop_surface *desktop_surface, void *shell) {
    debug << "desktop_surface_added" << std::endl;
    core->add_view(desktop_surface);
}
void desktop_surface_removed(weston_desktop_surface *surface, void *user_data) {
    debug << "desktop_surface_removed" << std::endl;
    /* TODO: what do we do when a view is destroyed ? */
}

void desktop_surface_commited (weston_desktop_surface *desktop_surface,
        int32_t sx, int32_t sy, void *data) {
    debug << "desktop_surface_committed" << std::endl;

    auto view = core->find_view(desktop_surface);
    assert(view != nullptr);

    if (view->surface->width == 0) {
        return;
    }

    if (!weston_surface_is_mapped(view->surface)) {
        view->map(sx, sy);
        //return;
    }

    /* TODO: handle fullscreen and maximized state
     * weston_desktop_surface_get_fullscreen() && weston_desktop_surface_get_maximized() */


    debug << "listing outputs" << std::endl;
    weston_output *output;
    wl_list_for_each(output, &core->ec->output_list, link) {
        weston_output_schedule_repaint(output);
        debug << "output id: " << output->id << std::endl;
    }
}

void desktop_surface_set_xwayland_position(weston_desktop_surface *desktop_surface,
        int32_t x, int32_t y, void *shell) {

    auto view = core->find_view(desktop_surface);
    assert(view != nullptr);

    view->xwayland.is_xorg = true;
    view->xwayland.x = x;
    view->xwayland.y = y;
}

#endif /* end of include guard: DESKTOP_API_HPP */