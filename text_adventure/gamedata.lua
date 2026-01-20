-- This is gamedata.lua
-- It contains all the game's logic and data.

-- Define the game world
-- This is a 'table' (like a map or dictionary)
Rooms = {
    ["start"] = {
        desc = "You are in a small, damp starting room. A wooden door is to the north.",
        exits = {
            north = "hallway",
            east = "closet"
        }
    },
    ["hallway"] = {
        desc = "You are in a long, dark hallway. The starting room is south. A large portcullis blocks the way north.",
        exits = {
            south = "start"
        },
        -- Consequences!
        consequences = {
            use_key = "You use the key on the portcullis. It groans and slowly rises! The path north is open.",
            set_exit = { north = "throne_room" }
        }
    },
    ["closet"] = {
        desc = "A small, dusty closet. You see a rusty 'key' on a shelf.",
        exits = {
            west = "start"
        },
        items = { "key" }
    },
    ["throne_room"] = {
        desc = "You've entered the throne room! You win!",
        exits = {}
    }
}

-- Our simple inventory
Inventory = {}

---
-- Returns the description for a room
--
function get_description(room_name)
    if Rooms[room_name] then
        local desc = Rooms[room_name].desc
        -- Check for items in the room
        if Rooms[room_name].items and #Rooms[room_name].items > 0 then
            desc = desc .. "\nItems here: "
            for i, item in ipairs(Rooms[room_name].items) do
                desc = desc .. item .. " "
            end
        end
        return desc
    else
        return "You are in a void. Something is wrong."
    end
end

---
-- Processes the user's command
-- Returns the name of the new room (or the same room if move fails)
--
function process_command(current_room_name, command)
    local room = Rooms[current_room_name]
    
    -- Simple parser: "verb object"
    local verb, object = string.match(command, "(%w+)%s*(.*)")
    
    -- 1. Movement
    if verb == "go" then
        if room.exits[object] then
            print("You go " .. object .. ".")
            return room.exits[object] -- Return new room
        else
            print("You can't go that way.")
            return current_room_name -- Return same room
        end
        
    -- 2. Take items
    elseif verb == "take" then
        local found = false
        if room.items then
            for i, item in ipairs(room.items) do
                if item == object then
                    print("You take the " .. object .. ".")
                    table.insert(Inventory, object) -- Add to inventory
                    table.remove(room.items, i)     -- Remove from room
                    found = true
                    break
                end
            end
        end
        if not found then
            print("You don't see a " .. object .. " here.")
        end
        return current_room_name -- Stay in same room

    -- 3. Use items (consequences)
    elseif verb == "use" then
        -- Check if item is in inventory
        local has_item = false
        for i, item in ipairs(Inventory) do
            if item == object then
                has_item = true
                break
            end
        end
        
        if has_item then
            -- Check if this room has a consequence for this item
            if room.consequences and room.consequences["use_" .. object] then
                print(room.consequences["use_" .. object])
                -- Apply the consequence
                if room.consequences.set_exit then
                    for dir, dest in pairs(room.consequences.set_exit) do
                        room.exits[dir] = dest
                    end
                end
            else
                print("You can't use the " .. object .. " here.")
            end
        else
            print("You don't have a " .. object .. ".")
        end
        return current_room_name -- Stay in same room

    -- 4. Unknown
    else
        print("I don't understand '" .. command .. "'. Try 'go', 'take', or 'use'.")
        return current_room_name -- Stay in same room
    end
end
