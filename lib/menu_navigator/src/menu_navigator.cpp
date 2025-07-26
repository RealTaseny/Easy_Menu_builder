//
// Created by Taseny on 25-7-21.
//

#include <cstring>
#include <string>

#include "../include/menu_navigator.h"

namespace Menu
{
    /**
     * @author Taseny
     * @brief Changeable items increment func
     */
    template <typename T>
    void ChangeableItem<T>::increment()
    {
        if (ref_ + step_ <= max_)
        {
            ref_ += step_;
            if (onChange_) onChange_(ref_);
        }
    }


    /**
     * @brief Changeable items decrement func
     */
    template <typename T>
    void ChangeableItem<T>::decrement()
    {
        if (ref_ - step_ >= min_)
        {
            ref_ -= step_;
            if (onChange_) onChange_(ref_);
        }
    }


    /**
     * @author Taseny
     * @brief Changeable items get display str func
     * @param value_str: The pointer to str buffer you want show
     * @param size: The max size of the buffer zone
     */
    template <typename T>
    void ChangeableItem<T>::getDisplayValue(char* value_str, const size_t size) const
    {
        memset(value_str, '\0', size);

        if constexpr (std::is_same_v<T, uint8_t> || std::is_same_v<T, uint16_t>)
            std::snprintf(value_str, size, "%u", ref_);
        else if constexpr (std::is_same_v<T, uint32_t>)
            std::snprintf(value_str, size, "%lu", ref_);
        else if constexpr (std::is_same_v<T, uint64_t>)
            std::snprintf(value_str, size, "%llu", ref_);
        else if constexpr (std::is_same_v<T, int8_t> || std::is_same_v<T, int16_t>)
            std::snprintf(value_str, size, "%d", ref_);
        else if constexpr (std::is_same_v<T, int32_t>)
            std::snprintf(value_str, size, "%d", ref_);
        else if constexpr (std::is_same_v<T, int64_t>)
            std::snprintf(value_str, size, "%d", ref_);
        else if constexpr (std::is_same_v<T, float>)
            std::snprintf(value_str, size, "%.3f", ref_);
        else if constexpr (std::is_same_v<T, double>)
            std::snprintf(value_str, size, "%.6f", ref_);
        else
            std::snprintf(value_str, size, "<?>"); // Invalid data type
    }


    /**
     * @author Taseny
     * @class menuItem: This is a class for Menu items
     * @param item_name: The name of the item
     * @param children_items: The pointer array to its children items
     * @param children_count: It determinate how many children its have. Note: This param of Changeable item and Toggle item should be zero(0)
     * @param type: Which type will this item be created.
     * @param app_func: If an item is normal and its children count is zero, this param control if it is app.
     * @param app_args: Application parameters pointer array
     */
    menuItem::menuItem(const char* item_name, menuItem** children_items, const uint8_t children_count,
                       const menuItemType type, void (*app_func)(void**), void** app_args)
        : item_name_(item_name),
          parent_item_(nullptr),
          children_item_(children_items),
          children_count_(children_count),
          saved_selected_index_(0),
          saved_first_visible_item_(0),
          type_(type)
    {
        if (children_items) // if the pointer array isn't empty, initialization those children items.
        {
            for (uint8_t i = 0; i < children_count; ++i)
            {
                children_items[i]->parent_item_ = this;
            }
        }
        else
        {
            if (type_ == menuItemType::NORMAL)
            {
                app_func_ = app_func;
                app_args_ = app_args;
            }
        }
    }


    /**
     * @author Taseny
     * @brief This is a factory method to create the `Normal` item
     * @param name: Name give to item
     * @param children_items: The pointer array for item's children
     * @param count: It determinate how many children the item has.
     * @return Pointer to a new item
     */
    menuItem* menuItem::createNormalItem(const char* name, menuItem** children_items, const uint8_t count)
    {
        return new menuItem(name, children_items, count, menuItemType::NORMAL);
    }


    /**
     * @author Taseny
     * @brief This is a factory method to create the `Changeable` item
     * @param name: Name give to item
     * @param ref: ref to val
     * @param min: The minimum value data can transition.
     * @param max: The maximum value data can transition.
     * @param step: Data transition step.
     * @param onChange: lamda func (callback) [It will be executed when item be changed]
     * @return Pointer to a new item
     */
    template <typename T>
    menuItem* menuItem::createChangeableItem(const char* name, T& ref, T min, T max, T step,
                                             void (*onChange)(T))
    {
        auto* item = new menuItem(name, nullptr, 0, menuItemType::CHANGEABLE);
        item->changeable_item_ = std::make_unique<ChangeableItem<T>>(ref, min, max, step, onChange);
        return item;
    }


    /**
     * @author Taseny
     * @brief This is a factory method to create the `Changeable` item
     * @param name: Name give to item
     * @param ref: ref to val
     * @param onToggle: lamda func (callback) [It will be executed when item be toggled]
     * @return Pointer to a new item
     */
    menuItem* menuItem::createToggle(const char* name, bool* ref, void (onToggle)(bool))
    {
        auto* item = new menuItem(name, nullptr, 0, menuItemType::TOGGLE);
        item->toggle_data_.ref = ref;
        item->toggle_data_.state = *ref;
        item->toggle_data_.onToggle = onToggle;
        return item;
    }


    /**
     *
     * @param name Name of application
     * @param app_func Pointer to target application func
     * @param app_args Parameters pointer array for application.
     * @return Pointer to a new item
     */
    menuItem* menuItem::createApp(const char* name, void** app_args, void (*app_func)(void**))
    {
        return new menuItem(name, nullptr, 0, menuItemType::NORMAL, app_func, app_args);
    }


    /**
     * @author Taseny
     * @brief toggle item's state (Only works for the `Toggle` item)
     */
    void menuItem::toggle()
    {
        if (type_ == menuItemType::TOGGLE && toggle_data_.ref)
        {
            *toggle_data_.ref = !(*toggle_data_.ref);
            toggle_data_.state = *toggle_data_.ref;
            if (toggle_data_.onToggle)
                toggle_data_.onToggle(*toggle_data_.ref);
        }
    }


    /**
     * @brief increase change item's data
     */
    void menuItem::increment() const
    {
        if (changeable_item_)
            changeable_item_->increment();
    }


    /**
     * @brief decrease change item's data
     */
    void menuItem::decrement() const
    {
        if (changeable_item_)
            changeable_item_->decrement();
    }


    /**
     * @author Taseny
     * @brief Items get display str func
     * @param value_str: The pointer to str buffer you want show
     * @param size: The max size of the buffer zone
     */
    void menuItem::getValueStr(char* value_str, const size_t size) const
    {
        if (type_ == menuItemType::TOGGLE)
        {
            memset(value_str, '\0', size);
            if (toggle_data_.state == true)
            {
                value_str[0] = 'O';
                value_str[1] = 'N';
                value_str[2] = ' ';
            }
            else
            {
                value_str[0] = 'O';
                value_str[1] = 'F';
                value_str[2] = 'F';
            }
        }
        if (type_ == menuItemType::CHANGEABLE && changeable_item_)
            changeable_item_->getDisplayValue(value_str, size);
    }


    void Navigator::handleInput(const keyValue& key_value)
    {
        if (in_app_mode_)
        {
            switch (key_value)
            {
            case keyValue::LEFT:
                in_app_mode_ = false;
                break;
            default:
                break;
            }
            return;
        }

        menuItem* point_item = current_menu_->children_item_[selected_index_];

        switch (key_value)
        {
        case keyValue::UP:
            switch (point_item->type_)
            {
            case menuItemType::NORMAL:
                selected_index_ = (selected_index_ == 0)
                                      ? (current_menu_->children_count_ - 1)
                                      : (selected_index_ - 1);
                if (selected_index_ == current_menu_->children_count_ - 1)
                {
                    if (current_menu_->children_count_ > MAX_DISPLAY_ITEM)
                    {
                        memset(display_buffer_, '\0', MAX_DISPLAY_CHAR * MAX_DISPLAY_ITEM);
                    }
                    first_visible_item_ = selected_index_ - (selected_index_ % MAX_DISPLAY_ITEM);
                }
                break;
            case menuItemType::CHANGEABLE:
                if (point_item->is_locked == false)
                    point_item->increment();
                else
                {
                    selected_index_ = (selected_index_ == 0)
                                          ? (current_menu_->children_count_ - 1)
                                          : (selected_index_ - 1);
                    if (selected_index_ == current_menu_->children_count_ - 1)
                    {
                        if (current_menu_->children_count_ > MAX_DISPLAY_ITEM)
                        {
                            memset(display_buffer_, '\0', MAX_DISPLAY_CHAR * MAX_DISPLAY_ITEM);
                        }
                        first_visible_item_ = selected_index_ - (selected_index_ % MAX_DISPLAY_ITEM);
                    }
                }
                break;
            case menuItemType::TOGGLE:
                if (point_item->is_locked == false)
                    point_item->toggle();
                else
                {
                    selected_index_ = (selected_index_ == 0)
                                          ? (current_menu_->children_count_ - 1)
                                          : (selected_index_ - 1);
                    if (selected_index_ == current_menu_->children_count_ - 1)
                    {
                        if (current_menu_->children_count_ > MAX_DISPLAY_ITEM)
                        {
                            memset(display_buffer_, '\0', MAX_DISPLAY_CHAR * MAX_DISPLAY_ITEM);
                        }
                        first_visible_item_ = selected_index_ - (selected_index_ % MAX_DISPLAY_ITEM);
                    }
                }

                break;
            default: ;
            }
            break;

        case keyValue::DOWN:
            switch (point_item->type_)
            {
            case menuItemType::NORMAL:
                selected_index_ = (selected_index_ + 1) % current_menu_->children_count_;
                if (selected_index_ == 0)
                {
                    if (current_menu_->children_count_ > MAX_DISPLAY_ITEM)
                    {
                        memset(display_buffer_, '\0', MAX_DISPLAY_CHAR * MAX_DISPLAY_ITEM);
                    }
                    first_visible_item_ = 0;
                }
                break;
            case menuItemType::CHANGEABLE:
                if (point_item->is_locked == false)
                    point_item->decrement();
                else
                {
                    selected_index_ = (selected_index_ + 1) % current_menu_->children_count_;
                    if (selected_index_ == 0)
                    {
                        if (current_menu_->children_count_ > MAX_DISPLAY_ITEM)
                        {
                            memset(display_buffer_, '\0', MAX_DISPLAY_CHAR * MAX_DISPLAY_ITEM);
                        }
                        first_visible_item_ = 0;
                    }
                }

                break;
            case menuItemType::TOGGLE:
                if (point_item->is_locked == false)
                    point_item->toggle();
                else
                {
                    selected_index_ = (selected_index_ + 1) % current_menu_->children_count_;
                    if (selected_index_ == 0)
                    {
                        if (current_menu_->children_count_ > MAX_DISPLAY_ITEM)
                        {
                            memset(display_buffer_, '\0', MAX_DISPLAY_CHAR * MAX_DISPLAY_ITEM);
                        }
                        first_visible_item_ = 0;
                    }
                }
                break;
            default: ;
            }
            break;

        case keyValue::RIGHT:

            switch (point_item->type_)
            {
            case menuItemType::NORMAL:
                if (point_item->children_count_ > 0)
                {
                    memset(display_buffer_, '\0', MAX_DISPLAY_CHAR * MAX_DISPLAY_ITEM);

                    current_menu_->saved_selected_index_ = selected_index_;
                    current_menu_->saved_first_visible_item_ = first_visible_item_;

                    current_menu_ = point_item;
                    selected_index_ = 0;
                    first_visible_item_ = 0;
                }
                else if (point_item->app_func_ != nullptr)
                {
                    memset(display_buffer_, '\0', MAX_DISPLAY_CHAR * MAX_DISPLAY_ITEM);

                    in_app_mode_ = true;

                    app_saved_selected_index_ = selected_index_;

                    point_item->app_func_(point_item->app_args_);
                }
                break;
            case menuItemType::CHANGEABLE:
            case menuItemType::TOGGLE:
                point_item->is_locked = false;
                break;
            default: ;
            }
            break;

        case keyValue::LEFT:
            switch (point_item->type_)
            {
            case menuItemType::NORMAL:
                if (current_menu_->parent_item_)
                {
                    memset(display_buffer_, '\0', MAX_DISPLAY_CHAR * MAX_DISPLAY_ITEM);

                    current_menu_ = current_menu_->parent_item_;
                    selected_index_ = current_menu_->saved_selected_index_;
                    first_visible_item_ = current_menu_->saved_first_visible_item_;
                }
                break;
            case menuItemType::CHANGEABLE:
            case menuItemType::TOGGLE:
                if (point_item->is_locked == false) point_item->is_locked = true;
                else
                {
                    if (current_menu_->parent_item_)
                    {
                        memset(display_buffer_, '\0', MAX_DISPLAY_CHAR * MAX_DISPLAY_ITEM);

                        current_menu_ = current_menu_->parent_item_;
                        selected_index_ = current_menu_->saved_selected_index_;
                        first_visible_item_ = current_menu_->saved_first_visible_item_;
                    }
                }
                break;
            }

            break;
        default: ;
        }

        if (current_menu_->children_count_ > MAX_DISPLAY_ITEM)
        {
            if (selected_index_ >= first_visible_item_ + MAX_DISPLAY_ITEM)
            {
                memset(display_buffer_, '\0', MAX_DISPLAY_CHAR * MAX_DISPLAY_ITEM);
                first_visible_item_ += MAX_DISPLAY_ITEM;
            }
            else if (selected_index_ < first_visible_item_ && selected_index_ != 0)
            {
                memset(display_buffer_, '\0', MAX_DISPLAY_CHAR * MAX_DISPLAY_ITEM);
                first_visible_item_ -= MAX_DISPLAY_ITEM;
            }
        }
    }

    void Navigator::refreshDisplay()
    {
        if (this->current_menu_ == nullptr)
        {
            memset(display_buffer_, '\0', MAX_DISPLAY_CHAR * MAX_DISPLAY_ITEM);
            std::snprintf(display_buffer_, MAX_DISPLAY_CHAR * MAX_DISPLAY_ITEM, "No Menu Item");
            return;
        }

        static char str[MAX_DISPLAY_CHAR] = {0};
        if (!in_app_mode_)
        {
            if (current_menu_->children_count_ - first_visible_item_ < MAX_DISPLAY_ITEM)
            {
                for (uint8_t i = 0; i < (current_menu_->children_count_ % MAX_DISPLAY_ITEM); i++)
                {
                    if (current_menu_->children_item_[i + first_visible_item_]->type_ == menuItemType::NORMAL)
                    {
                        std::snprintf(&display_buffer_[i * MAX_DISPLAY_CHAR], MAX_DISPLAY_CHAR, "%s%s",
                                      selected_index_ - first_visible_item_ == i ? "->" : "  ",
                                      current_menu_->children_item_[i + first_visible_item_]->item_name_);
                    }
                    else if (current_menu_->children_item_[i + first_visible_item_]->type_ == menuItemType::CHANGEABLE
                        ||
                        current_menu_->children_item_[i + first_visible_item_]->type_ == menuItemType::TOGGLE)
                    {
                        if (current_menu_->children_item_[i + first_visible_item_]->is_locked == true)
                        {
                            current_menu_->children_item_[i + first_visible_item_]->getValueStr(str, MAX_DISPLAY_CHAR);
                            std::snprintf(&display_buffer_[i * MAX_DISPLAY_CHAR], MAX_DISPLAY_CHAR, "%s%s: %s",
                                          selected_index_ - first_visible_item_ == i ? "->" : "  ",
                                          current_menu_->children_item_[i + first_visible_item_]->item_name_, str);
                        }
                        else
                        {
                            current_menu_->children_item_[i + first_visible_item_]->getValueStr(str, MAX_DISPLAY_CHAR);
                            std::snprintf(&display_buffer_[i * MAX_DISPLAY_CHAR], MAX_DISPLAY_CHAR, "%s%s: %s",
                                          selected_index_ - first_visible_item_ == i ? ">>" : "  ",
                                          current_menu_->children_item_[i + first_visible_item_]->item_name_, str);
                        }
                    }
                }

                memset(str, '\0', sizeof(str));
            }
            else
            {
                for (uint8_t i = 0; i < (current_menu_->children_count_ > MAX_DISPLAY_ITEM
                                     ? MAX_DISPLAY_ITEM
                                     : current_menu_->children_count_); i++)
                {
                    if (current_menu_->children_item_[i + first_visible_item_]->type_ == menuItemType::NORMAL)
                    {
                        std::snprintf(&display_buffer_[i * MAX_DISPLAY_CHAR], MAX_DISPLAY_CHAR, "%s%s",
                                      selected_index_ - first_visible_item_ == i ? "->" : "  ",
                                      current_menu_->children_item_[i + first_visible_item_]->item_name_);
                    }
                    else if (current_menu_->children_item_[i + first_visible_item_]->type_ == menuItemType::CHANGEABLE
                        ||
                        current_menu_->children_item_[i + first_visible_item_]->type_ == menuItemType::TOGGLE)
                    {
                        if (current_menu_->children_item_[i + first_visible_item_]->is_locked == true)
                        {
                            current_menu_->children_item_[i + first_visible_item_]->getValueStr(str, MAX_DISPLAY_CHAR);
                            std::snprintf(&display_buffer_[i * MAX_DISPLAY_CHAR], MAX_DISPLAY_CHAR, "%s%s: %s",
                                          selected_index_ - first_visible_item_ == i ? "->" : "  ",
                                          current_menu_->children_item_[i + first_visible_item_]->item_name_, str);
                        }
                        else
                        {
                            current_menu_->children_item_[i + first_visible_item_]->getValueStr(str, MAX_DISPLAY_CHAR);
                            std::snprintf(&display_buffer_[i * MAX_DISPLAY_CHAR], MAX_DISPLAY_CHAR, "%s%s: %s",
                                          selected_index_ - first_visible_item_ == i ? ">>" : "  ",
                                          current_menu_->children_item_[i + first_visible_item_]->item_name_, str);
                        }
                    }
                }

                memset(str, '\0', sizeof(str));
            }

        }
    }

    /// Explicit instantiation data type
    template menuItem* menuItem::createChangeableItem<uint8_t>(
        const char*, uint8_t&, uint8_t, uint8_t, uint8_t, void (*)(uint8_t));

    template menuItem* menuItem::createChangeableItem<uint16_t>(
        const char*, uint16_t&, uint16_t, uint16_t, uint16_t, void (*)(uint16_t));

    template menuItem* menuItem::createChangeableItem<uint32_t>(
        const char*, uint32_t&, uint32_t, uint32_t, uint32_t, void (*)(uint32_t));

    template menuItem* menuItem::createChangeableItem<uint64_t>(
        const char*, uint64_t&, uint64_t, uint64_t, uint64_t, void (*)(uint64_t));


    template menuItem* menuItem::createChangeableItem<int8_t>(
        const char*, int8_t&, int8_t, int8_t, int8_t, void (*)(int8_t));

    template menuItem* menuItem::createChangeableItem<int16_t>(
        const char*, int16_t&, int16_t, int16_t, int16_t, void (*)(int16_t));

    template menuItem* menuItem::createChangeableItem<int32_t>(
        const char*, int32_t&, int32_t, int32_t, int32_t, void (*)(int32_t));

    template menuItem* menuItem::createChangeableItem<int64_t>(
        const char*, int64_t&, int64_t, int64_t, int64_t, void (*)(int64_t));


    template menuItem* menuItem::createChangeableItem<float>(
        const char*, float&, float, float, float, void (*)(float));

    template menuItem* menuItem::createChangeableItem<double>(
        const char*, double&, double, double, double, void (*)(double));
}
