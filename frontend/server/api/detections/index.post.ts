import { z } from 'zod'
import { useSorterState } from '../utils/sorter-state'
import { useDetectionStore } from '../utils/detection-store'

const detectionSchema = z.object({
    partId: z.string().min(1),
    name: z.string().min(1),
    colour: z.string().min(1),
    confidence: z.number().min(0).max(1),
    binId: z.string().min(1),
    imageUrl: z.string().url().optional()
})

export default defineEventHandler(async (event) => {
    const body = await readValidatedBody(
        event,
        detectionSchema.parse
    )

    const detection = {
        id: crypto.randomUUID(),
        ...body,
        detectedAt: new Date().toISOString()
    }

    const detections = useDetectionStore()

    // Newest detection appears first.
    detections.unshift(detection)

    // Avoid retaining an unlimited number of detections.
    if (detections.length > 100) {
        detections.pop()
    }

    const sorter = useSorterState()
    sorter.bricksProcessed++

    return {
        success: true,
        detection
    }
})