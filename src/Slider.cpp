#include "Slider.h"

using namespace std;

Slider::Slider(
    shared_ptr<Module> content,
    SlideOrigin origin,
    EventType event,
    double slide_time)
    : Module(), m_content(content), m_origin(origin), m_slide_time(slide_time),
      m_slide_start(0), m_slide_event(event), m_slide_state(SLIDE_CLOSED),
      m_scratch(nullptr) {
    int x, y, w, h;
    content->get_location(x, y);
    content->get_size(w, h);
    move(x, y);
    resize(w, h);
}

Slider::~Slider() {
    if (m_scratch != nullptr) {
        al_destroy_bitmap(m_scratch);
    }
}

bool
Slider::on_update() {
    if (al_get_time() > (m_slide_start + m_slide_time)) {
        if (m_slide_state == SLIDE_OPENING) {
            m_slide_state = SLIDE_OPEN;
        } else if (m_slide_state == SLIDE_CLOSING) {
            m_slide_state = SLIDE_CLOSED;
        }
    }
    return m_content->update();
}

bool
Slider::on_draw(ALLEGRO_BITMAP *target) {
    int x, y;
    int w, h;

    if (m_slide_state == SLIDE_CLOSED) {
        return true;
    } else if (m_slide_state == SLIDE_OPEN) {
        return m_content->draw(target);
    }

    m_content->get_location(x, y);
    m_content->get_size(w, h);

    if (m_scratch
        && (al_get_bitmap_width(m_scratch) != x + w
            || al_get_bitmap_height(m_scratch) != y + h)) {
        al_destroy_bitmap(m_scratch);
        m_scratch = nullptr;
    }

    if (!m_scratch) {
        m_scratch = al_create_bitmap(x + w, y + h);
    }

    // render content to a scratch bitmap
    al_set_target_bitmap(m_scratch);
    al_clear_to_color(al_map_rgba(0, 0, 0, 0));
    m_content->draw(m_scratch);

    double now = al_get_time();
    double elapsed = now - m_slide_start;
    double partial = elapsed / m_slide_time;
    if (partial > 1) {
        partial = 1;
    } else if (partial < 0) {
        partial = 0;
    }
    if (m_slide_state == SLIDE_CLOSED || m_slide_state == SLIDE_CLOSING) {
        partial = 1 - partial;
    }
    al_set_target_bitmap(target);

    float source_x, source_y;
    float dest_x, dest_y;
    float source_w, source_h;

    switch (m_origin) {
    case SLIDE_FROM_LEFT:
        source_x = x + (1.0 - partial) * w;
        source_y = y;
        source_w = partial * w;
        source_h = h;
        dest_x = x;
        dest_y = y;
        break;
    case SLIDE_FROM_RIGHT:
        source_x = x;
        source_y = y;
        source_w = partial * w;
        source_h = h;
        dest_x = x + (1 - partial) * w;
        dest_y = y;
        break;
    case SLIDE_FROM_TOP:
        source_x = x;
        source_y = y + (1.0 - partial) * h;
        source_w = w;
        source_h = partial * h;
        dest_x = x;
        dest_y = y;
        break;
    case SLIDE_FROM_BOTTOM:
        source_x = x;
        source_y = y;
        source_w = w;
        source_h = partial * h;
        dest_x = x;
        dest_y = y + (1 - partial) * h;
        break;
    }
    al_draw_bitmap_region(
        m_scratch, source_x, source_y, source_w, source_h, dest_x, dest_y, 0);
    return true;
}

void
Slider::toggle() {
    if (m_slide_state == SLIDE_OPEN || m_slide_state == SLIDE_OPENING) {
        begin_close();
    } else {
        begin_open();
    }
}

ALLEGRO_MOUSE_EVENT
Slider::translate_mouse_coords(const ALLEGRO_MOUSE_EVENT *event) const {
    double partial = (al_get_time() - m_slide_start) / m_slide_time;

    ALLEGRO_MOUSE_EVENT translated = *event;

    switch (m_slide_state) {
    case SLIDE_OPEN:
        return translated;
    case SLIDE_CLOSED:
        translated.x = -1;
        translated.y = -1;
        return translated;
    case SLIDE_OPENING:
    case SLIDE_CLOSING:
        break;
    }

    switch (m_origin) {
    case SLIDE_FROM_LEFT:
        translated.x += m_width - static_cast<int>(m_width * partial);
        break;
    case SLIDE_FROM_RIGHT:
        translated.x -= static_cast<int>(m_width * partial);
        break;
    case SLIDE_FROM_TOP:
        translated.y += m_height - static_cast<int>(m_height * partial);
        break;
    case SLIDE_FROM_BOTTOM:
        translated.y -= static_cast<int>(m_height * partial);
    }
    return translated;
}

void
Slider::begin_open() {
    double now = al_get_time();
    double elapsed;

    switch (m_slide_state) {
    case SLIDE_OPENING:
    case SLIDE_OPEN:
        return;
    case SLIDE_CLOSING:
        elapsed = m_slide_time - (now - m_slide_start);
        break;
    case SLIDE_CLOSED:
        elapsed = 0;
    }

    double partial = elapsed / m_slide_time;
    if (partial > 1) {
        partial = 1;
    } else if (partial < 0) {
        partial = 0;
    }
    m_slide_start = now - m_slide_time * partial;
    m_slide_state = SLIDE_OPENING;
}

void
Slider::begin_close() {
    double now = al_get_time();
    double elapsed;

    switch (m_slide_state) {
    case SLIDE_CLOSING:
    case SLIDE_CLOSED:
        return;
    case SLIDE_OPENING:
        elapsed = m_slide_time - (now - m_slide_start);
        break;
    case SLIDE_OPEN:
        elapsed = 0;
    }

    double partial = elapsed / m_slide_time;
    if (partial > 1) {
        partial = 1;
    } else if (partial < 0) {
        partial = 0;
    }
    m_slide_start = now - m_slide_time * partial;
    m_slide_state = SLIDE_CLOSING;
}
// vi: ft=cpp
