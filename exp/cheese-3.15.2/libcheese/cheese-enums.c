


#include "cheese-enums.h"

/* enumerations from "./libcheese/cheese-widget.h" */
#include "./libcheese/cheese-widget.h"
GType
cheese_widget_state_get_type (void)
{
  static GType type = 0;

  if (!type)
  {
    static const GEnumValue _cheese_widget_state_values[] = {
      { CHEESE_WIDGET_STATE_NONE, "CHEESE_WIDGET_STATE_NONE", "none" },
      { CHEESE_WIDGET_STATE_READY, "CHEESE_WIDGET_STATE_READY", "ready" },
      { CHEESE_WIDGET_STATE_ERROR, "CHEESE_WIDGET_STATE_ERROR", "error" },
      { 0, NULL, NULL }
    };

    type = g_enum_register_static ("CheeseWidgetState", _cheese_widget_state_values);
  }

  return type;
}




