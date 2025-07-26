//
// Created by Taseny on 25-7-21.
//

#ifndef MENU_NAVIGATOR_H
#define MENU_NAVIGATOR_H

/**
 * Display API Define Zone
 */
#define MAX_DISPLAY_CHAR 21U
#define MAX_DISPLAY_ITEM 5U

/**
 * Display API Define Zone
 */

#ifdef __cplusplus

#include <memory>
#include <cstring>

namespace Menu
{
    enum class menuItemType
    {
        NORMAL,
        CHANGEABLE,
        TOGGLE
    };

    enum class keyValue
    {
        UP,
        DOWN,
        LEFT,
        RIGHT,
        NONE
    };

    class Changeable
    {
    public:
        virtual ~Changeable() = default;
        virtual void increment() = 0;
        virtual void decrement() = 0;
        virtual void getDisplayValue(char* value_str, size_t size) const = 0;
    };

    template <typename T>
    class ChangeableItem : public Changeable
    {
    public:
        T& ref_;
        T min_;
        T max_;
        T step_;
        void (*onChange_)(T);

        ChangeableItem(T& ref, T min, T max, T step, void (*callback)(T) = nullptr)
            : ref_(ref), min_(min), max_(max), step_(step), onChange_(callback)
        {
        }

        void increment() override;

        void decrement() override;

        void getDisplayValue(char* value_str, size_t size) const override;
    };

    class menuItem
    {
    public:
        using ptrChangeableItem = std::unique_ptr<Changeable>;

        bool is_locked = true;
        const char* item_name_;
        menuItem* parent_item_;
        menuItem** children_item_;
        uint8_t children_count_;
        uint8_t saved_selected_index_;
        uint8_t saved_first_visible_item_;
        menuItemType type_;

        ptrChangeableItem changeable_item_;

        void (*app_func_)(void**);
        void** app_args_;

        struct ToggleData
        {
            bool state;
            bool* ref;
            void (*onToggle)(bool);

            ToggleData()
            {
                state = false, ref = nullptr;
                onToggle = nullptr;
            }
        } toggle_data_;

        menuItem(const char* item_name, menuItem** children_items, uint8_t children_count, menuItemType type,
                 void (*app_func)(void**) = nullptr, void** app_args = nullptr);

        static menuItem* createNormalItem(const char* name, menuItem** children_items, uint8_t count);

        template <typename T>
        static menuItem* createChangeableItem(const char* name, T& ref, T min, T max, T step,
                                              void (*onChange)(T));

        static menuItem* createToggle(const char* name, bool* ref, void (onToggle)(bool));

        static menuItem* createApp(const char* name, void** app_args, void (*app_func)(void**));

        void toggle();

        void increment() const;

        void decrement() const;

        void getValueStr(char* value_str, size_t size) const;
    };

    class Navigator
    {
    public:
        explicit Navigator(menuItem* main_item)
        {
            current_menu_ = main_item;
            in_app_mode_ = false;
            selected_index_ = 0;
            first_visible_item_ = 0;
            app_saved_selected_index_ = 0;
        }

        void handleInput(const keyValue& key_value);

        void refreshDisplay();

        void setAppMode(const bool is_app_mode) { in_app_mode_ = is_app_mode; }

        bool getAppMode() const { return in_app_mode_; }

        char* getDisplayBuffer()
        {
            return display_buffer_;
        }

        void writeDisplayBuffer(const char* buffer, const size_t size, const uint8_t first_line)
        {
            memset(display_buffer_, '\0', MAX_DISPLAY_CHAR * MAX_DISPLAY_ITEM);
            memcpy(&display_buffer_[MAX_DISPLAY_CHAR * first_line], buffer, size);
        }

    private:
        char display_buffer_[MAX_DISPLAY_CHAR * MAX_DISPLAY_ITEM] = {0};

        menuItem* current_menu_;
        bool in_app_mode_;
        uint8_t selected_index_;
        uint8_t first_visible_item_;

        uint8_t app_saved_selected_index_;
    };
}

#endif

#endif //MENU_NAVIGATOR_H
