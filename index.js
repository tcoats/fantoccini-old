const w = [
  [1, 1, 1, 1, 1, 1, 1, 1],
  [1, 0, 0, 0, 0, 0, 0, 1],
  [1, 0, 0, 0, 0, 0, 0, 1],
  [1, 0, 0, 0, 0, 0, 0, 1],
  [1, 0, 0, 0, 0, 0, 0, 1],
  [1, 0, 0, 0, 0, 0, 0, 1],
  [1, 0, 0, 0, 0, 0, 0, 1],
  [1, 0, 0, 0, 0, 0, 0, 1],
  [1, 1, 1, 1, 1, 1, 1, 1]
]
const wdim = [w.length, w[0].length]

const p = [5, 5, 1, 0]

const canmove = () => {
  const n = [p[0] + p[2], p[1] + p[3]]
  return n[0] >= 0 && n[0] < wdim[0]
    && n[1] >= 0 && n[1] < wdim[1]
    && w[n[0]][n[1]] == 0
}
const move = () => {
  p[0] += p[2]
  p[1] += p[3]
}
const left = () => {
  const n = [-p[3], p[2]]
  p[2] = n[0]
  p[3] = n[1]
}
const right = () => {
  const n = [-p[3], p[2]]
  p[2] = n[0]
  p[3] = n[1]
}
const write = (x) => { process.stdout.write(x) }
const print = () => {
  w.forEach((row, x) => {
    row.forEach((cell, y) => {
      if (x != p[0] || y != p[1])
        write(`${cell} `)
      else if (p[2] == 1)
        write('v ')
      else if (p[2] == -1)
        write('^ ')
      else if (p[3] == 1)
        write('> ')
      else if (p[3] == -1)
        write('< ')
    })
    write('\n')
  })
}
