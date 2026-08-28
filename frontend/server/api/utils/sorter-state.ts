export type SorterMode =
    | 'idle'
    | 'starting'
    | 'running'
    | 'stopping'
    | 'fault'

export interface SorterState {
    mode: SorterMode
    sessionId: string | null
    startedAt: string | null
    bricksProcessed: number
    bricksPerMinute: number
    lastError: string | null
}

const sorterState: SorterState = {
    mode: 'idle',
    sessionId: null,
    startedAt: null,
    bricksProcessed: 0,
    bricksPerMinute: 0,
    lastError: null
}

export function useSorterState() {
    return sorterState
}