#include <stdio.h>
#include <stdlib.h>

void selectiveRepeat(int total_frames, int window_size)
{
    int *ack = (int *)calloc(total_frames, sizeof(int));
    int i = 0;
    while (i < total_frames)
    {
        printf("Transmitting frames: ");
        for (int j = i; j < i + window_size && j < total_frames; j++)
        {
            if (!ack[j])
                printf("%d ", j + 1);
        }

        printf("\n");
        int received_frame;
        printf("Enter the frame number acknowledged: ");
        scanf("%d", &received_frame);

        if (received_frame < 1 || received_frame > total_frames)
        {
            printf("Invalid frame number. Please enter a number between 1 and %d.\n", total_frames);
            continue;
        }

        ack[received_frame - 1] = 1;
        int all_acked = 1;
        for (int j = i; j < i + window_size && j < total_frames; j++)
        {
            if (!ack[j])
            {
                all_acked = 0;
                break;
            }
        }
        if (all_acked)
            i += window_size;
        else
            printf("Resending unacknowledged frames\n");
    }
    free(ack);
}

int main()
{
    int total_frames = 10;
    int window_size = 4;
    selectiveRepeat(total_frames, window_size);
    return 0;
}