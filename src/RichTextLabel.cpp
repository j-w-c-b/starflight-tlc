#include "RichTextLabel.h"

using namespace std;

RichTextLabel::RichTextLabel(
    int x,
    int y,
    int width,
    int height,
    bool multiline,
    int flags,
    std::shared_ptr<ALLEGRO_FONT> font,
    ALLEGRO_COLOR color,
    ALLEGRO_COLOR background_color,
    const std::vector<RichText> &text)
    : Module(x, y, width, height), m_multiline(multiline), m_font(font),
      m_default_color(color), m_default_background_color(background_color),
      m_dirty(true), m_flags(flags) {
    set_text(text);
    RichTextLabel::resize(get_width(), get_height());
    m_dirty = true;
}

void
RichTextLabel::resize(int width, int height) {
    if (!m_canvas || width != get_width() || height != get_height()) {
        m_canvas = std::shared_ptr<ALLEGRO_BITMAP>(
            al_create_bitmap(width, height), al_destroy_bitmap);
        if (!m_canvas_bitmap) {
            m_canvas_bitmap =
                make_shared<Bitmap>(m_canvas, get_x(), get_y(), width, height);
            add_child_module(m_canvas_bitmap);
        } else {
            m_canvas_bitmap->resize(width, height);
            m_canvas_bitmap->set_bitmap(m_canvas);
        }
        m_dirty = true;
    }
    Module::resize(width, height);
}

bool
RichTextLabel::on_draw([[maybe_unused]] ALLEGRO_BITMAP *target) {
    refresh();
    return true;
}

bool
RichTextLabel::on_close() {
    remove_child_module(m_canvas_bitmap);
    m_canvas = nullptr;
    m_canvas_bitmap = nullptr;
    return true;
}

bool
RichTextLabel::draw_line(
    int line_num,
    const char *line,
    int size,
    void *extra) {
    auto state = static_cast<RichTextLabel::RefreshState *>(extra);
    string partial = {line, static_cast<string::size_type>(size)};
    int partial_offset = 0;
    float x_offset = 0;

    state->current_y_offset =
        line_num * al_get_font_line_height(state->t->m_font.get());

    if (state->t->m_flags == ALLEGRO_ALIGN_RIGHT) {
        x_offset = state->t->get_width()
                   - al_get_text_width(state->t->m_font.get(), partial.c_str());
    } else if (state->t->m_flags == ALLEGRO_ALIGN_CENTER) {
        x_offset =
            (state->t->get_width()
             - al_get_text_width(state->t->m_font.get(), partial.c_str()))
            / 2.0;
    }

    while (partial != "") {
        int remaining =
            state->it->m_text.length() - state->current_string_offset;

        while (remaining == 0) {
            ++state->it;
            if (state->it == state->end) {
                goto done;
            }
            state->current_string_offset = 0;
            remaining = state->it->m_text.length();
        }
        if (remaining > size - partial_offset) {
            remaining = size - partial_offset;
        }
        string substr = {
            line + partial_offset, static_cast<string::size_type>(remaining)};
        al_draw_text(
            state->t->m_font.get(),
            state->it->m_color,
            x_offset + state->current_x_offset,
            state->current_y_offset,
            0,
            substr.c_str());
        state->current_x_offset +=
            al_get_text_width(state->t->m_font.get(), substr.c_str());
        partial_offset += remaining;
        state->current_string_offset += remaining;
        partial = {
            line + partial_offset,
            static_cast<string::size_type>(size - partial_offset)};
    }
    // Skip over leading '\n', ' ', or '\t'
    while (state->it != state->end) {
        int current_length = state->it->m_text.length();
        int remaining = current_length - state->current_string_offset;

        ALLEGRO_ASSERT(remaining >= 0);
        while (remaining == 0) {
            ++state->it;
            state->current_string_offset = 0;
            if (state->it == state->end) {
                goto done;
            }
            remaining = state->it->m_text.length();
        }

        while (
            remaining > 0 && state->current_string_offset < current_length
            && (state->it->m_text[state->current_string_offset] == ' '
                || state->it->m_text[state->current_string_offset] == '\n'
                || state->it->m_text[state->current_string_offset] == '\t')) {
            remaining--;
            state->current_string_offset++;
        }
        if (remaining > 0) {
            goto done;
        }
    }
done:
    state->current_x_offset = 0;
    return true;
}

bool
RichTextLabel::count_line(
    int line_num,
    const char *line,
    int size,
    void *extra) {
    auto state = static_cast<RichTextLabel::RefreshState *>(extra);
    state->current_y_offset =
        (line_num + 1) * al_get_font_line_height(state->t->m_font.get());
    return true;
}

void
RichTextLabel::refresh() {
    if (!m_dirty) {
        return;
    }
    al_set_target_bitmap(m_canvas.get());
    al_clear_to_color(m_default_background_color);

    if (!m_multiline) {
        int offset = 0;

        for (auto &i : m_text) {
            al_draw_text(
                m_font.get(), i.m_color, offset, 0, 0, i.m_text.c_str());
            offset += al_get_text_width(m_font.get(), i.m_text.c_str());
        }
    } else {
        string all_text = accumulate(
            m_text.cbegin(),
            m_text.cend(),
            string(""),
            [](const string &accum, const RichText &rt) {
                return accum + rt.m_text;
            });

        RefreshState r = {this, 0, m_text.cbegin(), m_text.cend(), 0, 0, 0};

        al_do_multiline_text(
            m_font.get(), get_width(), all_text.c_str(), draw_line, &r);
    }

    m_dirty = false;
}
// vi: ft=cpp
