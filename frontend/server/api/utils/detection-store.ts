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

// replace that with db items
const detections: Detection[] = []

export function useDetectionStore() {
    return detections
}