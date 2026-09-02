/**
 * Simple debounce utility for Vue components.
 * Returns a debounced version of the callback that delays execution
 * until `wait` ms have elapsed since the last call.
 */
export function debounce<TArgs extends any[]>(
  fn: (...args: TArgs) => void,
  wait: number,
): (...args: TArgs) => void {
  let timer: ReturnType<typeof setTimeout> | null = null
  return (...args: TArgs) => {
    if (timer) {
      clearTimeout(timer)
    }
    timer = setTimeout(() => {
      timer = null
      fn(...args)
    }, wait)
  }
}
