export interface SorterStatus {
    mode: 'idle' | 'starting' | 'running' | 'stopping' | 'fault'
    sessionId: string | null
    startedAt: string | null
    bricksProcessed: number
    bricksPerMinute: number
    lastError: string | null
}

export interface Detection {
    id: string
    partId: string
    name: string
    colour: string
    confidence: number
    binId: string
    imageUrl?: string
    detectedAt: string
}