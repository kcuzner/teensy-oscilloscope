Building an oscilloscope

1. The DMA is complicated. Really complicated.
2. DMAMUX is used to cnofigure mappings into each DMA channel
    - ADC0 and ADC1 are two potential sources
    - ADCx_SC3.DMAEN should allow the ADC to request a DMA transfer
    - Each DMA channel selects a source, the source numbers are listed @3.3.8.1
3. eDMA is the actual DMA controller
    - While the DMAMUX just controls who requests the DMA action, the DMA
      transfer needs to be configred propertly
    - The transfer size needs to be 32 bit (ATTR reg)
    - Source address needs to be set to ADCx_Rn (A or B...I think)
        - The source address offset is 0, same with slast
            - These could be used to handle the ADCx_Rn situation
        - All of the nbytes registers probably need to be 1
    - Destination address needs to be set to wherever we want the result to be
      written to
        - The destination offset should be 4 since each transfer is 4 bytes
    - The CITER registers should probably also be 1 since only one transfer
      happens at a time

Making this into an oscilloscope
- According to the datasheet, it is nearly impossible to get full conversion
  speeds without using the DMA
- Requirements:
    - Trigger & delay capability
    - Have a bit of a "preview" before the trigger event (a "lead-in")
- Memory:
    - We have several lengthy buffers which represent the "width" of our
      oscilloscope screen. These will be called the sample buffers.
        - These are either free or in-use. A buffer is marked in-use by the
          ADC portion and a buffer is marked free by the USB module once it
          completes a transfer. If no free buffers are found, the ADC will
          wait until one becomes free.
    - One free running buffer which is configured as a circular buffer in the
      DMA. Normally, the ADC writes to this buffer
    - When a trigger is detected (in ADC ISR), a delay countdown begins. Each
      sample afterwards, the delay counter is decremented (all the while still
      writing to the circular buffer)
    - Once the delay counter is set to 0, two things happens:
        - The ADC's DMA channel is switched to write instead to the next free
          sample buffer, starting a point N inside the buffer
        - A DMA channel is configured to copy from the circular buffer N bytes
          before the last source address into the sample buffer. It will fill in
          the gap between where the ADC is writing and the beginning of the
          buffer. This gives us our "lead-in".
            - It may be more intuitive to have the delay "cut in" to the lead
              time (i.e. a lead time of 20 samples becomes a lead time of 5 if
              the delay is 15)
        - If no free sample buffers are found, we are now limited by the USB
          module. We start over, writing to the circular buffer waiting for a
          trigger event
    - Every write to the sample buffer also needs to be written to the circular
      buffer to give the appropriate lead-in to the next trigger event
    - After we reach the end of the sample buffer, we start over, waiting for a
      trigger event
- DMA setup:
    - One channel is always writing to the circular buffer, it has medium
      priority. ADC conversion triggered.
    - Another is set up to write to the sample buffer, it has high priority. ADC
      conversion triggered.
    - A third channel copies from the circular buffer into the sample buffer, it
      has high priority. Software triggered.
- USB setup:
    - Endpoint 0 controls the parameters of the oscilloscope:
        - Sample rate
        - Trigger edge and level
        - etc
    - A bulk IN endpoint will communicate the contents of the buffers.
    - Upon receipt of an IN token, the endpoint will either:
        - If no buffer is currently being transmitted, find the next buffer and
          set it up to be transmitted in chunks (packet size is limited to 64
          bytes).
        - If a buffer is currently being transmitted, transmit the next chunk
        - If no buffer is being transmitted and no buffers are ready to be
          transmitted, transfer a packet of 0 length.
- Host:
    - Every couple milliseconds, attempt a bulk read of the data endpoint
