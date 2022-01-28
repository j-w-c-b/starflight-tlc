ModuleCaptainAttribute::ModuleCaptainAttribute(
    const string &name,
    int value,
    int max,
    int x,
    int y,
    int width,
    int height,
    EventType plus_event,
    EventType minus_event,
    ResourceManager<ALLEGRO_BITMAP> &resources)
    : Module(x, y, width, height), m_name_label(new Label(
                                       name,
                                       x,
                                       y,
                                       ATTR_LABEL_WIDTH,
                                       ATTR_LABEL_HEIGHT,
                                       false,
                                       0,
                                       g_game->font32,
                                       TEXTCOL)),
      m_value_label(new Label(
          to_string(value),
          x + ATTR_LABEL_WIDTH,
          y,
          ATTR_VALUE_WIDTH,
          ATTR_LABEL_HEIGHT,
          0,
          ALLEGRO_ALIGN_RIGHT,
          g_game->font32,
          TEXTCOL)),
      m_plus_button(new NewButton(
          x + ATTR_LABEL_WIDTH + ATTR_VALUE_WIDTH + ATTR_VALUE_PADDING,
          y,
          ATTR_PLUS_WIDTH,
          ATTR_PLUS_HEIGHT,
          EVENT_NONE,
          plus_event,
          resources[I_CAPTAINCREATION_PLUS],
          resources[I_CAPTAINCREATION_PLUS_MOUSEOVER])),
      m_minus_button(new NewButton(
          x + ATTR_LABEL_WIDTH + ATTR_VALUE_WIDTH + ATTR_VALUE_PADDING
              + ATTR_PLUS_WIDTH + ATTR_PLUS_PADDING,
          y,
          ATTR_MINUS_WIDTH,
          ATTR_MINUS_HEIGHT,
          EVENT_NONE,
          minus_event,
          resources[I_CAPTAINCREATION_PLUS],
          resources[I_CAPTAINCREATION_PLUS_MOUSEOVER])),
      m_value(value), m_min(value), m_max(max) {
    add_child_module(m_name_label);
    add_child_module(m_value_label);
    add_child_module(m_plus_button);
    add_child_module(m_minus_button);
}

ModuleCaptainAttributeGroup::ModuleCaptainAttributeGroup(
    int x,
    int y,
    int width,
    int height,
    int max,
    ResourceManager<ALLEGRO_BITMAP> &resources)
    : Module(x, y, width, height), m_sum(0), m_max(max), m_resources(resources),
      m_available_label(new Label(
          to_string(max) + " available",
          x + ATTR_LABEL_X + ATTR_LABEL_WIDTH + ATTR_VALUE_WIDTH
              + ATTR_VALUE_PADDING + ATTR_PLUS_WIDTH + ATTR_PLUS_PADDING
              + ATTR_MINUS_WIDTH + ATTR_MINUS_PADDING,
          y + height / 2,
          ATTR_AVAILABLE_WIDTH,
          ATTR_LABEL_HEIGHT,
          false,
          ALLEGRO_ALIGN_RIGHT,
          g_game->font32,
          TEXTCOL)) {
    add_child_module(m_available_label);
}

bool
ModuleCaptainAttributeGroup::on_event(ALLEGRO_EVENT *event) {
    EventType t = static_cast<EventType>(event->type);

    auto attribute = m_attributes_by_plus.find(t);
    if (attribute != m_attributes_by_plus.end()) {
        if (m_sum > m_max || !attribute->second->plus()) {
            g_game->audioSystem->Play(S_ERROR);
        } else {
            ++m_sum;
            g_game->audioSystem->Play(S_BUTTONCLICK);
        }
        return false;
    }
    attribute = m_attributes_by_minus.find(t);
    if (attribute != m_attributes_by_minus.end()) {
        if (m_sum < 0 || !attribute->second->minus()) {
            g_game->audioSystem->Play(S_ERROR);
        } else {
            --m_sum;
            g_game->audioSystem->Play(S_BUTTONCLICK);
        }
        return false;
    }
    return true;
}

void
ModuleCaptainAttributeGroup::add_attribute(
    const std::string &name,
    int value,
    int max,
    EventType plus_event,
    EventType minus_event) {
    ModuleCaptainAttribute *a = new ModuleCaptainAttribute(
        name,
        value,
        max,
        m_x + ATTR_LABEL_X,
        m_y + ATTR_LABEL_HEIGHT * m_attributes.size(),
        ATTR_LABEL_WIDTH + ATTR_VALUE_WIDTH + ATTR_VALUE_PADDING
            + ATTR_PLUS_WIDTH + ATTR_PLUS_PADDING + ATTR_MINUS_WIDTH
            + ATTR_MINUS_PADDING + ATTR_AVAILABLE_WIDTH,
        ATTR_LABEL_HEIGHT,
        plus_event,
        minus_event,
        m_resources);
    m_attributes[name] = a;
    m_attributes_by_plus[plus_event] = a;
    m_attributes_by_minus[minus_event] = a;
}
