const w = [
  [
    [[0], [0], [0], [0], [0]],
    [[18, 0, 0, 1, 1, 0], [0], [0], [0], [0]],
    [[0], [0], [0], [0], [0]],
    [[18, 0, 0, 1, 1, 0], [0], [0], [0], [0]],
    [[0], [19], [0], [0], [0]]
  ]
]
const p = [0, 0, 0, 1, 0]
const v = [5]
const s = []
const h = []

const d = [w.length, w[0].length, w[0][0].length]
const clip = () => {
  p[0] = (p[0] + d[0]) % d[0]
  p[1] = (p[1] + d[1]) % d[1]
  p[2] = (p[2] + d[2]) % d[2]
}

const i = {
  0: (o) => [0], // noop
  1: (o) => { // left
    [p[3], p[4]] = [-p[4], p[3]]
    return [2]
  },
  2: (o) => { // right
    [p[3], p[4]] = [p[4], -p[3]]
    return [1]
  },
  3: (o) => { // reverse
    [p[3], p[4]] = [-p[3], -p[4]]
    return [3]
  },
  4: (o) => { // jump
    const undo = [4, p[0], p[1], p[2]];
    [p[0], p[1], p[2]] = [o[1], o[2], o[3]]
    clip()
    return undo
  },
  5: (o) => { // teleport
    const undo = [5, p[0], p[1], p[2], p[3], p[4]];
    [p[0], p[1], p[2], p[3], p[4]] = [o[1], o[2], o[3], o[4], o[5]]
    clip()
    return undo
  },
  6: (o) => { // turn
    const undo = [6, p[3], p[4]];
    [p[3], p[4]] = [o[1], o[2]]
    return undo
  },
  7: (o) => { // increment
    v[o[1]]++
    return [8, o[1]]
  },
  8: (o) => { // decrement
    v[o[1]]--
    return [9, o[1]]
  },
  9: (o) => { // copy
    const undo = [20, v[o[2]], o[2]]
    v[o[2]] = v[o[1]]
    return undo
  },
  10: (o) => { // add
    v[o[3]] = v[o[1]] + v[o[2]]
    return [11, o[1], o[2], o[3]]
  },
  11: (o) => { // subtract
    v[o[3]] = v[o[1]] - v[o[2]]
    return [10, o[1], o[2], o[3]]
  },
  12: (o) => { // left if
    if (v[o[1]] > 0) [p[3], p[4]] = [-p[4], p[3]]
    return [13, o[1]]
  },
  13: (o) => { // right if
    if (v[o[1]] > 0) [p[3], p[4]] = [p[4], -p[3]]
    return [12, o[1]]
  },
  14: (o) => { // reverse if
    if (v[o[1]] > 0) [p[3], p[4]] = [-p[3], -p[4]]
    return [14, o[1]]
  },
  15: (o) => { // left if not
    if (v[o[1]] <= 0) [p[3], p[4]] = [-p[4], p[3]]
    return [16, o[1]]
  },
  16: (o) => { // right if not
    if (v[o[1]] <= 0) [p[3], p[4]] = [p[4], -p[3]]
    return [15, o[1]]
  },
  17: (o) => { // reverse if not
    if (v[o[1]] <= 0) [p[3], p[4]] = [-p[3], -p[4]]
    return [17, o[1]]
  },
  18: (o) => { // call
    if (s.length == 1000) throw new Exception()
    const undo = [19]
    s.push([p[0], p[1], p[2], p[3], p[4]]);
    [p[0], p[1], p[2], p[3], p[4]] = [o[1], o[2], o[3], o[4], o[5]]
    clip()
    return undo
  },
  19: (o) => { // return
    if (s.length == 0) throw new Exception()
    const undo = [18, p[0], p[1], p[2], p[3], p[4]];
    [p[0], p[1], p[2], p[3], p[4]] = s.pop()
    clip()
    return undo
  },
  20: (o) => { // set
    const undo = [20, v[o[2]], o[2]]
    v[o[2]] = o[1]
    return undo
  }
}

const forward = () => {
  [p[1], p[2]] = [p[1] + p[3], p[2] + p[4]]
  clip()
  const o = w[p[0]][p[1]][p[2]]
  h.push(i[o[0]](o))
}
const backward = () => {
  if (h.length == 0) return
  let o = h.pop()
  if (o == null) o = w[p[0]][p[1]][p[2]]
  i[o[0]](o);
  [p[1], p[2]] = [p[1] - p[3], p[2] - p[4]]
  clip()
}

const pad = (n, c) => {
  let result = n.toString()
  while (result.length < c) result += ' '
  return result
}
const write = (x) => { process.stdout.write(x) }
const print = () => {
  // v.forEach((i) => { write(`${i} `) })
  // write('\n')
  // write(s.length.toString())
  // write('\n')
  w.forEach((layer, z) => {
    layer.forEach((row, x) => {
      row.forEach((cell, y) => {
        if (x != p[1] || y != p[2] || z != p[0]) write(pad(cell[0], 3))
        else if (p[3] == 1) write('v  ')
        else if (p[3] == -1) write('^  ')
        else if (p[4] == 1) write('>  ')
        else if (p[4] == -1) write('<  ')
      })
      write('\n')
    })
  })
  write('\n')
}

print()
forward()
print()
forward()
print()
forward()
print()
forward()
print()
forward()
print()
backward()
print()
backward()
print()
backward()
print()
backward()
print()
backward()
print()
