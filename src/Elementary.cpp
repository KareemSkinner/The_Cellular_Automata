#include "Elementary.h"

// Default ruleset is rule 90. (https://mathworld.wolfram.com/ElementaryCellularAutomaton.html)
Elementary::Elementary()
    : m_cellMap()
    , m_ruleset("01011010")
    , m_numberOfCellsPerGeneration(200)
    , m_numberOfGenerations(500) {}

const std::map<ImVec2, CellState>& Elementary::GetCellMap() const
{
    return m_cellMap;
}

int Elementary::GetNumberOfCellsPerGeneration() const
{
    return m_numberOfCellsPerGeneration;
}
int Elementary::GetNumberOfGenerations() const
{
    return m_numberOfGenerations;
}

CellState Elementary::GetCellState(ImVec2 cell) const
{
    if (m_cellMap.find(cell) != m_cellMap.end()) {
        auto state = m_cellMap.at(cell);
        return state;
    }

    else {
        return CellState::inactive;
    }
}

std::bitset<8>& Elementary::SetRuleset()
{
    return m_ruleset;
}

void Elementary::SetNumberOfCellsPerGeneration(int input)
{
    if (input > 0)
        m_numberOfCellsPerGeneration = input;
}

void Elementary::SetNumberOfGenerations(int input)
{
    if (input > 0)
        m_numberOfGenerations = input;
}

bool Elementary::SetSingleCellState(ImVec2 cell, CellState state)
{
    if (m_cellMap.find(cell) != m_cellMap.end()) {
        m_cellMap[cell] = state;
        return true;
    } else {
        return false;
    }
}

void Elementary::GenerateCells(CellState state = CellState::inactive)
{
    m_cellMap.clear();

    for (int generation = 0; generation < m_numberOfGenerations; ++generation) {
        for (int position = 0; position < m_numberOfCellsPerGeneration; ++position) {
            ImVec2 cell = ImVec2(static_cast<float>(position), static_cast<float>(generation));
            m_cellMap.insert(std::make_pair(cell, state));
        }
    }
}

void Elementary::SetAllCellStates()
{
    for (auto& [cell, state] : m_cellMap) {
        // Excludes cells(x, 0) as this is the initial generation that contains a single active cell.
        if (cell.y != 0) {
            std::bitset<3> previousCells("000");

            // Note that bitset reads from right to left, so 0 position is the furthest right bit.
            ImVec2 prev_cell_0 = ImVec2(cell.x + 1, cell.y - 1);
            ImVec2 prev_cell_1 = ImVec2(cell.x, cell.y - 1);
            ImVec2 prev_cell_2 = ImVec2(cell.x - 1, cell.y - 1);

            // Checks previous neighbour cells' states, which determines the current cell's state based on the rule bitset (m_ruleset).
            const CellState defaultState = CellState::inactive;
            const CellState& state_0 = (m_cellMap.find(prev_cell_0) != m_cellMap.end()) ? m_cellMap.at(prev_cell_0) : defaultState;
            const CellState& state_1 = (m_cellMap.find(prev_cell_1) != m_cellMap.end()) ? m_cellMap.at(prev_cell_1) : defaultState;
            const CellState& state_2 = (m_cellMap.find(prev_cell_2) != m_cellMap.end()) ? m_cellMap.at(prev_cell_2) : defaultState;

            if (state_0 == CellState::active)
                previousCells[0].flip();
            if (state_1 == CellState::active)
                previousCells[1].flip();
            if (state_2 == CellState::active)
                previousCells[2].flip();

            // For comparison using a switch statement.
            auto neighbourCellsState = previousCells.to_string();

            // This is the table that specifies a cell's state given its previous state, and the states of the cells to the left & right.
            const std::string rulePosition_0 = "111";
            const std::string rulePosition_1 = "110";
            const std::string rulePosition_2 = "101";
            const std::string rulePosition_3 = "100";
            const std::string rulePosition_4 = "011";
            const std::string rulePosition_5 = "010";
            const std::string rulePosition_6 = "001";
            const std::string rulePosition_7 = "000";

            // Can't switch on a string, so they're mapped to ints instead.
            const std::map<std::string, int> ruleMap = {
                { rulePosition_0, 0 },
                { rulePosition_1, 1 },
                { rulePosition_2, 2 },
                { rulePosition_3, 3 },
                { rulePosition_4, 4 },
                { rulePosition_5, 5 },
                { rulePosition_6, 6 },
                { rulePosition_7, 7 }
            };

            switch (ruleMap.at(neighbourCellsState)) {
            case 0: {
                state = static_cast<CellState>(m_ruleset.test(0));
                break;
            }
            case 1: {
                state = static_cast<CellState>(m_ruleset.test(1));
                break;
            }
            case 2: {
                state = static_cast<CellState>(m_ruleset.test(2));
                break;
            }
            case 3: {
                state = static_cast<CellState>(m_ruleset.test(3));
                break;
            }
            case 4: {
                state = static_cast<CellState>(m_ruleset.test(4));
                break;
            }
            case 5: {
                state = static_cast<CellState>(m_ruleset.test(5));
                break;
            }
            case 6: {
                state = static_cast<CellState>(m_ruleset.test(6));
                break;
            }
            case 7: {
                state = static_cast<CellState>(m_ruleset.test(7));
                break;
            }
            }
        }
    }
}

void Elementary::DrawCells()
{
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    draw_list->PushClipRect(m_min_canvas_position, m_max_canvas_position, true);

    const ImVec2 origin = ImVec2(m_min_canvas_position.x + m_grid_scrolling.x, m_min_canvas_position.y + m_grid_scrolling.y);
    draw_list->AddRect(origin, ImVec2(origin.x + (m_numberOfCellsPerGeneration * m_grid_steps), origin.y + (m_numberOfGenerations * m_grid_steps)), m_cell_colour_main);

    for (const auto& [cell, state] : m_cellMap) {
        if (state == CellState::active) {
            const ImVec2 cell_pos_i = ImVec2(origin.x + (cell.x * m_grid_steps), origin.y + (cell.y * m_grid_steps));
            const ImVec2 cell_pos_f = ImVec2(cell_pos_i.x + m_grid_steps, cell_pos_i.y + m_grid_steps);
            draw_list->AddRectFilled(cell_pos_i, cell_pos_f, m_cell_colour_main);
        }
    }

    draw_list->PopClipRect();
}

void Elementary::GenerateElementaryAutomata()
{
    GenerateCells();
    const float startPoint = static_cast<float>(m_numberOfCellsPerGeneration) / 2.0f;
    SetSingleCellState(ImVec2(startPoint, 0), CellState::active);
    SetAllCellStates();
}
