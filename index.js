const w = [
  [
    [[0], [0], [0], [0], [5, 0, 0, 0, 1, 0]],
    [[0], [0], [0], [0], [0]],
    [[0], [0], [0], [0], [0]],
    [[0], [0], [0], [0], [0]],
    [[5, 0, 0, 0, 0, 1], [0], [0], [0], [0]]
  ]
]
const d = [w.length, w[0].length, w[0][0].length]
const p = [0, 0, 0, 1, 0]

const clip = () => {
  p[0] = (p[0] + d[0]) % d[0]
  p[1] = (p[1] + d[1]) % d[1]
  p[2] = (p[2] + d[2]) % d[2]
}
const forward = () => {
  p[1] += p[3]
  p[2] += p[4]
  clip()
}
const identity = () => {}
const left = () => {
  [p[3], p[4]] = [-p[4], p[3]]
}
const right = () => {
  [p[3], p[4]] = [p[4], -p[3]]
}
const reverse = () => {
  [p[3], p[4]] = [-p[3], -p[4]]
}
const jump = (z, x, y) => {
  [p[0], p[1], p[2]] = [z, x, y]
  clip()
}
const teleport = (z, x, y, i, j) => {
  [p[0], p[1], p[2], p[3], p[4]] = [z, x, y, i, j]
  clip()
}

const i = {
  0: () => { identity() },
  1: () => { left() },
  2: () => { right() },
  3: () => { reverse() },
  4: (o) => { jump(o[1], o[2], o[3]) },
  5: (o) => { teleport(o[1], o[2], o[3], o[4], o[5]) }
}

const step = () => {
  forward()
  const o = w[p[0]][p[1]][p[2]]
  i[o[0]](o)
}

const write = (x) => { process.stdout.write(x) }
const print = () => {
  w.forEach((layer, z) => {
    layer.forEach((row, x) => {
      row.forEach((cell, y) => {
        if (x != p[1] || y != p[2] || z != p[0])
          write(`${cell} `)
        else if (p[3] == 1)
          write('v ')
        else if (p[3] == -1)
          write('^ ')
        else if (p[4] == 1)
          write('> ')
        else if (p[4] == -1)
          write('< ')
      })
      write('\n')
    })
  })
  write('\n')
}

setInterval(() => {
  step()
  print()
}, 500)
