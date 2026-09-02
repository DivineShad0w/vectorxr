import assert from 'node:assert/strict'
import test from 'node:test'

import { createHeadCursorProfile, defaultConfig, defaultHeadCursorConfig, normalizeConfig } from '../src/lib/model.ts'
import { validateConfig } from '../src/lib/validation.ts'

const msfs = { id: 'msfs2024', name: 'MSFS 2024', enabled: true, match: { exe: 'FlightSimulator.exe' } }

test('configs written before Head Cursor load with it off and default settings', () => {
  const config = defaultConfig()
  delete config.modules.headCursor
  const normalized = normalizeConfig(config)
  assert.deepEqual(normalized.modules.headCursor, { enabled: false, defaults: defaultHeadCursorConfig(), profiles: [] })
  assert.deepEqual(validateConfig(normalized), [])
})

test('a Head Cursor profile for a registered application saves clean', () => {
  const config = defaultConfig()
  config.applications.push(msfs)
  config.modules.headCursor.enabled = true
  config.modules.headCursor.profiles.push(createHeadCursorProfile(defaultHeadCursorConfig(), [msfs.id]))
  assert.deepEqual(validateConfig(config), [])
})

test('out-of-range Head Cursor settings are reported for the defaults and for a profile', () => {
  const config = defaultConfig()
  config.applications.push(msfs)
  const bad = {
    ...defaultHeadCursorConfig(),
    yawSensitivity: 0,
    pitchSensitivity: 6,
    yawMultiplier: 0.4,
    pitchMultiplier: 11,
    deadzoneDegrees: 6,
    maxMovePerFrame: 49,
  }
  config.modules.headCursor.defaults = bad
  const profile = createHeadCursorProfile(defaultHeadCursorConfig(), [msfs.id])
  profile.settings = bad
  config.modules.headCursor.profiles.push(profile)

  const errors = validateConfig(config)
  for (const key of ['yawSensitivity', 'pitchSensitivity', 'yawMultiplier', 'pitchMultiplier', 'deadzoneDegrees', 'maxMovePerFrame']) {
    assert.ok(errors.some((error) => error.startsWith(`modules.headCursor.defaults.${key}`)), `defaults.${key} went unreported`)
    assert.ok(errors.some((error) => error.startsWith(`modules.headCursor.profiles[0].settings.${key}`)), `profiles[0].settings.${key} went unreported`)
  }
})

test('Head Cursor profiles name their applications once and never share one', () => {
  const config = defaultConfig()
  config.applications.push(msfs)
  const first = createHeadCursorProfile(defaultHeadCursorConfig(), [msfs.id, 'nope', msfs.id])
  first.name = '  '
  const second = createHeadCursorProfile(defaultHeadCursorConfig(), [msfs.id])
  config.modules.headCursor.profiles.push(first, second)

  const errors = validateConfig(config)
  assert.ok(errors.includes('modules.headCursor.profiles[0].name is required'))
  assert.ok(errors.includes('modules.headCursor.profiles[0].applicationIds references unknown application: nope'))
  assert.ok(errors.includes('modules.headCursor.profiles[0].applicationIds duplicates msfs2024'))
  assert.ok(errors.includes('modules.headCursor.profiles[1] conflicts with profiles[0] for application msfs2024; first enabled profile wins'))
})

test('a disabled Head Cursor profile cannot shadow another for the same application', () => {
  const config = defaultConfig()
  config.applications.push(msfs)
  const first = createHeadCursorProfile(defaultHeadCursorConfig(), [msfs.id])
  first.enabled = false
  const second = createHeadCursorProfile(defaultHeadCursorConfig(), [msfs.id])
  config.modules.headCursor.profiles.push(first, second)
  assert.deepEqual(validateConfig(config), [])
})
